#pragma once
#include <PxActor.h>
#include <PxAggregate.h>
#include <PxArticulation.h>
#include <PxArticulationBase.h>
#include <PxArticulationJoint.h>
#include <PxArticulationJointReducedCoordinate.h>
#include <PxArticulationLink.h>
#include <PxArticulationReducedCoordinate.h>
#include <PxBatchQuery.h>
#include <PxBatchQueryDesc.h>
#include <PxBroadPhase.h>
#include <PxClient.h>
#include <PxConfig.h>
#include <PxConstraint.h>
#include <PxConstraintDesc.h>
#include <PxContact.h>
#include <PxContactModifyCallback.h>
#include <PxDeletionListener.h>
#include <PxFiltering.h>
#include <PxForceMode.h>
#include <PxFoundation.h>
#include <PxImmediateMode.h>
#include <PxLockedData.h>
#include <PxMaterial.h>
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <PxPhysicsSerialization.h>
#include <PxPhysicsVersion.h>
#include <PxPhysXConfig.h>
#include <PxPruningStructure.h>
#include <PxQueryFiltering.h>
#include <PxQueryReport.h>
#include <PxRigidActor.h>
#include <PxRigidBody.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
#include <PxScene.h>
#include <PxSceneDesc.h>
#include <PxSceneLock.h>
#include <PxShape.h>
#include <PxSimulationEventCallback.h>
#include <PxSimulationStatistics.h>
#include <PxVisualizationParameter.h>

#include <characterkinematic/PxBoxController.h>
#include <characterkinematic/PxCapsuleController.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerBehavior.h>
#include <characterkinematic/PxControllerManager.h>
#include <characterkinematic/PxControllerObstacles.h>
#include <characterkinematic/PxExtended.h>

#include <collision/PxCollisionDefs.h>

#include <common/PxBase.h>
#include <common/PxCollection.h>
#include <common/PxCoreUtilityTypes.h>
#include <common/PxMetaData.h>
#include <common/PxMetaDataFlags.h>
#include <common/PxPhysicsInsertionCallback.h>
#include <common/PxPhysXCommonConfig.h>
#include <common/PxProfileZone.h>
#include <common/PxRenderBuffer.h>
#include <common/PxSerialFramework.h>
#include <common/PxSerializer.h>
#include <common/PxStringTable.h>
#include <common/PxTolerancesScale.h>
#include <common/PxTypeInfo.h>
#include <common/windows/PxWindowsDelayLoadHook.h>

#include <cooking/PxBVH33MidphaseDesc.h>
#include <cooking/PxBVH34MidphaseDesc.h>
#include <cooking/PxBVHStructureDesc.h>
#include <cooking/Pxc.h>
#include <cooking/PxConvexMeshDesc.h>
#include <cooking/PxCooking.h>
#include <cooking/PxMidphaseDesc.h>
#include <cooking/PxTriangleMeshDesc.h>

#include <cudamanager/PxCudaContextManager.h>
#include <cudamanager/PxCudaMemoryManager.h>

#include <extensions/PxBinaryConverter.h>
#include <extensions/PxBroadPhaseExt.h>
#include <extensions/PxCollectionExt.h>
#include <extensions/PxConstraintExt.h>
#include <extensions/PxContactJoint.h>
#include <extensions/PxConvexMeshExt.h>
#include <extensions/PxD6Joint.h>
#include <extensions/PxD6JointCreate.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <extensions/PxDefaultStreams.h>
#include <extensions/PxDistanceJoint.h>
#include <extensions/PxExtensionsAPI.h>
#include <extensions/PxFixedJoint.h>
#include <extensions/PxJoint.h>
#include <extensions/PxJointLimit.h>
#include <extensions/PxMassProperties.h>
#include <extensions/PxPrismaticJoint.h>
#include <extensions/PxRaycastCCD.h>
#include <extensions/PxRepXSerializer.h>
#include <extensions/PxRepXSimpleType.h>
#include <extensions/PxRevoluteJoint.h>
#include <extensions/PxRigidActorExt.h>
#include <extensions/PxRigidBodyExt.h>
#include <extensions/PxSceneQueryExt.h>
#include <extensions/PxSerialization.h>
#include <extensions/PxShapeExt.h>
#include <extensions/PxSimpleFactory.h>
#include <extensions/PxSmoothNormals.h>
#include <extensions/PxSphericalJoint.h>
#include <extensions/PxStringTableExt.h>
#include <extensions/PxTriangleMeshExt.h>

#include <filebuf/PxFileBuf.h>

#include <foundation/Px.h>
#include <foundation/PxAllocatorCallback.h>
#include <foundation/PxAssert.h>
#include <foundation/PxBitAndData.h>
#include <foundation/PxBounds3.h>
#include <foundation/PxErrorCallback.h>
#include <foundation/PxErrors.h>
#include <foundation/PxFlags.h>
#include <foundation/PxFoundationConfig.h>
#include <foundation/PxIntrinsics.h>
#include <foundation/PxIO.h>
#include <foundation/PxMat33.h>
#include <foundation/PxMat44.h>
#include <foundation/PxMath.h>
#include <foundation/PxMathUtils.h>
#include <foundation/PxMemory.h>
#include <foundation/PxPlane.h>
#include <foundation/PxPreprocessor.h>
#include <foundation/PxProfiler.h>
#include <foundation/PxQuat.h>
#include <foundation/PxSharedAssert.h>
#include <foundation/PxSimpleTypes.h>
#include <foundation/PxStrideIterator.h>
#include <foundation/PxTransform.h>
#include <foundation/PxUnionCast.h>
#include <foundation/PxVec2.h>
#include <foundation/PxVec3.h>
#include <foundation/PxVec4.h>
//#include <foundation/unix/PxUnixIntrinsics.h>
#include <foundation/windows/PxWindowsIntrinsics.h>

#include <geometry/PxBoxGeometry.h>
#include <geometry/PxBVHStructure.h>
#include <geometry/PxCapsuleGeometry.h>
#include <geometry/PxConvexMesh.h>
#include <geometry/PxConvexMeshGeometry.h>
#include <geometry/PxGeometry.h>
#include <geometry/PxGeometryHelpers.h>
#include <geometry/PxGeometryQuery.h>
#include <geometry/PxHeightField.h>
#include <geometry/PxHeightFieldDesc.h>
#include <geometry/PxHeightFieldFlag.h>
#include <geometry/PxHeightFieldGeometry.h>
#include <geometry/PxHeightFieldSample.h>
#include <geometry/PxMeshQuery.h>
#include <geometry/PxMeshScale.h>
#include <geometry/PxPlaneGeometry.h>
#include <geometry/PxSimpleTriangleMesh.h>
#include <geometry/PxSphereGeometry.h>
#include <geometry/PxTriangle.h>
#include <geometry/PxTriangleMesh.h>
#include <geometry/PxTriangleMeshGeometry.h>

#include <geomutils/GuContactBuffer.h>
#include <geomutils/GuContactPoint.h>

#include <gpu/PxGpu.h>

#include <pvd/PxPvd.h>
#include <pvd/PxPvdSceneClient.h>
#include <pvd/PxPvdTransport.h>

#include <solver/PxSolverDefs.h>

#include <task/PxCpuDispatcher.h>
#include <task/PxTask.h>
#include <task/PxTaskDefine.h>
#include <task/PxTaskManager.h>

#include <vehicle/PxVehicleComponents.h>
#include <vehicle/PxVehicleDrive.h>
#include <vehicle/PxVehicleDrive4W.h>
#include <vehicle/PxVehicleDriveNW.h>
#include <vehicle/PxVehicleDriveTank.h>
#include <vehicle/PxVehicleNoDrive.h>
#include <vehicle/PxVehicleSDK.h>
#include <vehicle/PxVehicleShaders.h>
#include <vehicle/PxVehicleTireFriction.h>
#include <vehicle/PxVehicleUpdate.h>
#include <vehicle/PxVehicleUtil.h>
#include <vehicle/PxVehicleUtilControl.h>
#include <vehicle/PxVehicleUtilSetup.h>
#include <vehicle/PxVehicleUtilTelemetry.h>
#include <vehicle/PxVehicleWheels.h>

namespace orbit {

	using namespace physx;

}
