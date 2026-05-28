#include <cmath>
#include <unordered_map>
#include <vector>
#include "CollisionSystem.hpp"
#include "LinearAlgebra.hpp"
#include "Primitives.hpp"
#include "World.hpp"

namespace Smol
{
    struct Contact {
        Vec2 normal; // 단위벡터, 이 body 기준 바깥 방향
        f32 penetration; // overlap 깊이 (>0)
    };

    constexpr f32 kEps = 1e-5f;

    static Vec2 ConeClip(Vec2 v, const std::vector<Contact>& contacts) {
        if (contacts.empty()) return v;

        auto isFeasible = [&contacts](Vec2 x) -> bool {
            for (const auto& c : contacts) {
                if (dot(x, c.normal) < -kEps) return false;
            }
            return true;
            };

        // already satisfy all constraints
        if (isFeasible(v)) return v;

        // for each unsatisfied i
        // projection to boundary
        Vec2 best = Vec2(0, 0);
        f32  bestDist2 = std::numeric_limits<f32>::infinity();
        bool found = false;

        for (const auto& c : contacts) {
            f32 d = dot(v, c.normal);
            if (d >= -kEps) continue;             // 위반 아닌 제약은 스킵

            Vec2 p = v - d * c.normal;            // boundary line으로 perpendicular foot
            if (!isFeasible(p)) continue;         // 다른 제약 위반 → 폐기

            f32 dist2 = d * d;                    // |p - v|^2 = dot(v, n_i)^2
            if (dist2 < bestDist2) {
                bestDist2 = dist2;
                best = p;
                found = true;
            }
        }
        return found ? best : Vec2(0, 0);
    }

    void CollisionSystem::Update(World& world) {
        std::unordered_map<EntityID, std::vector<Contact>> contactMap;

        // Phase 1: Collect Contact
        for (auto [id0, t0, v0, c0] : world.cenumerate<
            Transform, Velocity, SphereCollider
        >()) {
            for (auto [id1, t1, v1, c1] : world.cenumerate<
                Transform, Velocity, SphereCollider
            >().StartAt(id0 + 1)) {

                Vec2 delta = t1.position - t0.position;
                f32  distSq = normSquared(delta);
                f32  sumR = c0.radius + c1.radius;
                if (distSq >= sumR * sumR) continue;

                f32  dist = std::sqrt(distSq);
                Vec2 dir = (dist > kEps) ? delta / dist : Vec2(1, 0);  // body0→body1 (degenerate fallback)
                f32  pen = sumR - dist;

                contactMap[id0].push_back({ -dir, pen });   // body0 입장에선 body1 반대쪽이 outward
                contactMap[id1].push_back({ dir, pen });
            }
        }

        // ── Phase 2: body마다 separation + cone-clip
        constexpr f32 kSeparationBias = 10.0f;       // penetration → velocity 변환 rate
        constexpr f32 kMaxSeparationSpeed = 5.0f;    // 깊은 침투에서 폭발 방지

        for (auto [id, t, v, c] : world.enumerate<
            Transform, Velocity, SphereCollider
        >()) {
            auto it = contactMap.find(id);
            if (it == contactMap.end())
                continue;
            const auto& contacts = it->second;

            // (1) penalty-style separation: outward normal로 밀어내는 velocity 추가
            //     이건 정의상 cone에 feasible(dot(sep, n)>=0)이라 다음 단계에서 안 깎임
            for (const auto& contact : contacts) {
                f32 sepSpeed = std::min(contact.penetration * kSeparationBias, kMaxSeparationSpeed);
                v.value += contact.normal * sepSpeed;
            }

            // (2) 모든 contact 동시 만족 영역으로 cone-clip (기존 inward 성분 제거)
            v.value = ConeClip(v.value, contacts);
        }
    }
}
