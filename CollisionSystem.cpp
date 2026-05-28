#include <cmath>
#include <unordered_map>
#include <vector>
#include "CollisionSystem.hpp"
#include "LinearAlgebra.hpp"
#include "OS.hpp"
#include "Primitives.hpp"
#include "World.hpp"

namespace Smol
{
    constexpr f32 kEps = 1e-5f;
    constexpr int kVelIters = 6;
    constexpr int kPosIters = 3;
    constexpr f32 kRestThreshold = 50.0f;
    constexpr f32 kPenSlop = 0.5f;
    constexpr f32 kBaumgarte = 0.2f;

    struct SolverBody {
        Vec2  pos, vel;
        f32   radius, invMass, restitution, friction;
        Vec2* dstPos = nullptr;   // null = wall
        Vec2* dstVel = nullptr;
    };
    struct Contact {
        u32  a, b;
        Vec2 normal, tangent;     // a → b
        f32  penetration, velocityBias;
        f32  normalImpulse = 0.0f;
        f32  tangentImpulse = 0.0f;
    };

    void CollisionSystem::Update(World& world) {
        const f32 W = static_cast<f32>(OS_->GetWidth());
        const f32 H = static_cast<f32>(OS_->GetHeight());

        // 1. Collect SolverBody
        std::vector<SolverBody> bodies;
        for (auto [t, v, c, rb] : world.query<
            Transform, Velocity, SphereCollider, Rigidbody
        >()) {
            bodies.push_back(SolverBody{
                .pos = t.position,
                .vel = v.value,
                .radius = c.radius,

                .invMass = rb.invMass,
                .restitution = rb.restitution,
                .friction = rb.friction,

                .dstPos = &t.position,
                .dstVel = &v.value
                });
        }

        const u32 nDyn = (u32)bodies.size();
        const u32 wall = nDyn;
        constexpr f32 kWallRest = 0.1f;
        constexpr f32 kWallFriction = 0.5f;

        bodies.push_back(SolverBody{
            .pos = {0, 0},
            .vel = {0, 0},
            .radius = 0.0f,
            .invMass = 0.0f,
            .restitution = kWallRest,
            .friction = kWallFriction
        });

        // 2. Calculate Contact
        std::vector<Contact> contacts;
        auto make = [&](u32 a, u32 b, Vec2 n, f32 pen) {
            f32 e = std::min(bodies[a].restitution, bodies[b].restitution);
            f32 vn = dot(bodies[b].vel - bodies[a].vel, n);

            contacts.push_back(Contact{
                .a = a,
                .b = b,
                .normal = n,
                .tangent = Vec2(-n.y, n.x),
                .penetration = pen,
                .velocityBias = (vn < -kRestThreshold) ? -e * vn : 0.0f
            });
        };

        // 2.1. circle vs circle
        for (u32 i = 0; i < nDyn; ++i) {
            for (u32 j = i + 1; j < nDyn; ++j) {
                Vec2 d = bodies[j].pos - bodies[i].pos;
                f32  distSq = dot(d, d);
                f32  rsum = bodies[i].radius + bodies[j].radius;
                if (distSq >= rsum * rsum) continue;
                f32  dist = std::sqrt(distSq);
                Vec2 n = (dist > kEps) ? d / dist : Vec2(1, 0);  // i → j
                make(i, j, n, rsum - dist);
            }
        }

        // 2.2. circle vs wall
        for (u32 i = 0; i < nDyn; ++i) {
            f32 r = bodies[i].radius; Vec2 p = bodies[i].pos;
            if (p.y + r > H) make(i, wall, Vec2(0, 1), (p.y + r) - H);
            if (p.y - r < 0) make(i, wall, Vec2(0, -1), -(p.y - r));
            if (p.x - r < 0) make(i, wall, Vec2(-1, 0), -(p.x - r));
            if (p.x + r > W) make(i, wall, Vec2(1, 0), (p.x + r) - W);
        }

        // 3. Solve Velocity
        for (int it = 0; it < kVelIters; ++it) {
            for (auto& c : contacts) {
                auto& A = bodies[c.a]; auto& B = bodies[c.b];
                f32 invSum = A.invMass + B.invMass;
                if (invSum <= 0.0f) continue;

                // normal
                f32 vn = dot(B.vel - A.vel, c.normal);
                f32 dPn = -(vn - c.velocityBias) / invSum;
                f32 oldPn = c.normalImpulse;
                c.normalImpulse = std::max(oldPn + dPn, 0.0f);
                dPn = c.normalImpulse - oldPn;
                Vec2 Pn = dPn * c.normal;
                A.vel -= Pn * A.invMass;
                B.vel += Pn * B.invMass;

                // friction (Coulomb cone: |jt| <= mu * jn)
                f32 vt = dot(B.vel - A.vel, c.tangent);
                f32 dPt = -vt / invSum;
                f32 mu = std::sqrt(A.friction * B.friction);
                f32 maxF = mu * c.normalImpulse;
                f32 oldPt = c.tangentImpulse;
                c.tangentImpulse = std::clamp(oldPt + dPt, -maxF, maxF);
                dPt = c.tangentImpulse - oldPt;
                Vec2 Pt = dPt * c.tangent;
                A.vel -= Pt * A.invMass;
                B.vel += Pt * B.invMass;
            }
        }

        // 4. Correct Position (cached pen, Baumgarte split)
        for (int it = 0; it < kPosIters; ++it) {
            for (auto& c : contacts) {
                auto& A = bodies[c.a]; auto& B = bodies[c.b];
                f32 invSum = A.invMass + B.invMass;
                if (invSum <= 0.0f) continue;
                f32 corr = std::max(c.penetration - kPenSlop, 0.0f) / invSum * kBaumgarte;
                Vec2 P = corr * c.normal;
                A.pos -= P * A.invMass;
                B.pos += P * B.invMass;
            }
        }

        // 5. Write back
        for (auto& b : bodies) {
            if (b.dstVel) *b.dstVel = b.vel;
            if (b.dstPos) *b.dstPos = b.pos;
        }
    }
}
