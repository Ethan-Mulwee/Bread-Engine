#ifndef BENGINE_BODY
#define BENGINE_BODY

#include <bMath/bMath.hpp>

namespace bEngine {
	class RigidBody {
		public:
			float inverseMass;
			bMath::float3x3 inverseInertiaTensor;

			bMath::float3 position;
			bMath::quaternion orientation = bMath::quaternion(1,0,0,0);

			bMath::float3 linearVelocity;
			bMath::float3 angularVelocity;

			bMath::float3 forceAccum;
			bMath::float3 torqueAccum;
            

		public:
			RigidBody() {};

			bMath::matrix4 getTransform() const {
				bMath::float3x3 o = quaternionToMatrix(orientation);
				return bMath::matrix4(
					o(0,0), o(0,1), o(0,2), position.x,
					o(1,0), o(1,1), o(1,2), position.y,
					o(2,0), o(2,1), o(2,2), position.z,
					0,      0,      0,      1
				);
			}

			bMath::float3 positionToBodySpace(bMath::float3 wpos) {
				wpos -= position;
				bMath::float3x3 o = quaternionToMatrix(orientation);
				o = bMath::transpose(o);
				return wpos*o;
			}

			void addForce(const bMath::float3 &force);

			void addTorque(const bMath::float3 &torque);

			void addForceAtPoint(const bMath::float3 &force, const bMath::float3 &point);

			void addForceAtBodyPoint(const bMath::float3 &force, const bMath::float3 &point);

			void integrate(float time);
            
		private:
			void clearAccumlators();
    };
}

#endif