cmake_minimum_required(VERSION 3.30)
project(Black_Engine)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_TESTS "Build the test suite" ON)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif()

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)

add_library(glad STATIC
        external/glad/src/glad.c
)
target_include_directories(glad PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
)

# First define imgui library so ImGuizmo can find it
add_library(imgui STATIC
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include
)

# Now define ImGuizmo library after imgui is set up
add_library(ImGuizmo STATIC
        external/ImGuizmo/ImGuizmo.cpp
)

target_include_directories(ImGuizmo PUBLIC
        external/ImGuizmo
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
)

add_executable(Black_Engine
        src/main.cpp
        src/Application.cpp
        src/Application.h
        src/Engine/Render/Shader/Shader.h
        src/Engine/Render/Shader/Shader.cpp
        src/Engine/Render/Mesh/Mesh.cpp
        src/Engine/Render/Mesh/Mesh.h
        src/Engine/Render/Primitives/Primitives.cpp
        src/Engine/Render/Primitives/Primitives.h
        src/Engine/Scene/Scene.cpp
        src/Engine/Scene/Scene.h
        src/Engine/Component/BaseComponent.h
        src/Engine/Component/TransformComponent.cpp
        src/Engine/Component/TransformComponent.h
        src/Engine/Component/MeshRendererComponent.cpp
        src/Engine/Component/MeshRendererComponent.h
        src/Engine/Entity/GameObject.h
        src/Engine/Entity/GameObject.cpp
        src/Core/Camera/Camera.h
        src/Core/Camera/Camera.cpp
        src/Core/InputManager/InputManager.h
        src/Core/InputManager/InputManager.cpp
        src/Core/WindowManager/WindowManager.h
        src/Core/WindowManager/WindowManager.cpp
        src/Editor/UI/Panels/Panel.h
        src/Editor/UI/Panels/Panel.cpp
        src/Editor/UI/Layout/EditorLayout.h
        src/Editor/UI/Layout/EditorLayout.cpp
        src/Editor/UI/Panels/HierarchyPanel/HierarchyPanel.h
        src/Editor/UI/Panels/InspectorPanel/InspectorPanel.h
        src/Editor/UI/Panels/ScenePanel/ScenePanel.h
        src/Editor/UI/Panels/GamePanel/GamePanel.h
        src/Core/ImGui/ImGuiLayer.h
        src/Core/ImGui/ImGuiLayer.cpp
        src/Editor/UI/Panels/HierarchyPanel/HierarchyPanel.cpp
        src/Editor/UI/Panels/GamePanel/GamePanel.cpp
        src/Editor/UI/Panels/ScenePanel/ScenePanel.cpp
        src/Editor/UI/Panels/InspectorPanel/InspectorPanel.cpp
        src/Core/InputManager/InputEvent.h
        src/Core/InputSystem/InputSystem.h
        src/Core/InputSystem/InputSystem.cpp
        src/Core/InputManager/IInputEventReceiver.h
        src/Engine/Component/MeshComponent.h
        src/Engine/Component/MeshComponent.cpp
        src/Editor/UI/Panels/InspectorPanel/ComponentDrawers.h
        src/Editor/UI/Panels/InspectorPanel/ComponentDrawers.cpp
        src/Engine/Render/Texture/Texture.cpp
        src/Engine/Render/Texture/Texture.h
        src/Engine/Render/Mesh/VBO/VBO.cpp
        src/Engine/Render/Mesh/VBO/VBO.h
        src/Engine/Render/Mesh/VAO/VAO.cpp
        src/Engine/Render/Mesh/VAO/VAO.h
        src/Engine/Render/Mesh/EBO/EBO.cpp
        src/Engine/Render/Mesh/EBO/EBO.h
        src/Engine/Render/Material/Material.cpp
        src/Engine/Render/Material/Material.h
        src/Editor/SelectionManager.h
        src/Editor/SelectionManager.cpp

        src/Physics/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp
        src/Physics/BulletCollision/BroadphaseCollision/btAxisSweep3.h
        src/Physics/BulletCollision/BroadphaseCollision/btAxisSweep3Internal.h
        src/Physics/BulletCollision/BroadphaseCollision/btBroadphaseInterface.h
        src/Physics/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp
        src/Physics/BulletCollision/BroadphaseCollision/btBroadphaseProxy.h
        src/Physics/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp
        src/Physics/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h
        src/Physics/BulletCollision/BroadphaseCollision/btDbvt.cpp
        src/Physics/BulletCollision/BroadphaseCollision/btDbvt.h
        src/Physics/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp
        src/Physics/BulletCollision/BroadphaseCollision/btDbvtBroadphase.h
        src/Physics/BulletCollision/BroadphaseCollision/btDispatcher.cpp
        src/Physics/BulletCollision/BroadphaseCollision/btDispatcher.h
        src/Physics/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp
        src/Physics/BulletCollision/BroadphaseCollision/btOverlappingPairCache.h
        src/Physics/BulletCollision/BroadphaseCollision/btOverlappingPairCallback.h
        src/Physics/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp
        src/Physics/BulletCollision/BroadphaseCollision/btQuantizedBvh.h
        src/Physics/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp
        src/Physics/BulletCollision/BroadphaseCollision/btSimpleBroadphase.h

        src/Physics/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp
        src/Physics/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.h
        src/Physics/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp
        src/Physics/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.h
        src/Physics/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp
        src/Physics/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.h
        src/Physics/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp
        src/Physics/BulletCollision/CollisionDispatch/btBoxBoxDetector.h
        src/Physics/BulletCollision/CollisionDispatch/btCollisionConfiguration.h
        src/Physics/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp
        src/Physics/BulletCollision/CollisionDispatch/btCollisionDispatcher.h
        src/Physics/BulletCollision/CollisionDispatch/btCollisionCreateFunc.h
        src/Physics/BulletCollision/CollisionDispatch/btCollisionDispatcherMt.cpp
        src/Physics/BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h
        src/Physics/BulletCollision/CollisionDispatch/btCollisionObject.cpp
        src/Physics/BulletCollision/CollisionDispatch/btCollisionObject.h
        src/Physics/BulletCollision/CollisionDispatch/btCollisionObjectWrapper.h
        src/Physics/BulletCollision/CollisionDispatch/btCollisionWorld.cpp
        src/Physics/BulletCollision/CollisionDispatch/btCollisionWorld.h
        src/Physics/BulletCollision/CollisionDispatch/btCollisionWorldImporter.cpp
        src/Physics/BulletCollision/CollisionDispatch/btCollisionWorldImporter.h
        src/Physics/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp
        src/Physics/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.h
        src/Physics/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.cpp
        src/Physics/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.h
        src/Physics/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp
        src/Physics/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.h
        src/Physics/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp
        src/Physics/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.h
        src/Physics/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp
        src/Physics/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.h
        src/Physics/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp
        src/Physics/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.h
        src/Physics/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp
        src/Physics/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h
        src/Physics/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp
        src/Physics/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.h
        src/Physics/BulletCollision/CollisionDispatch/btGhostObject.cpp
        src/Physics/BulletCollision/CollisionDispatch/btGhostObject.h
        src/Physics/BulletCollision/CollisionDispatch/btHashedSimplePairCache.cpp
        src/Physics/BulletCollision/CollisionDispatch/btHashedSimplePairCache.h
        src/Physics/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp
        src/Physics/BulletCollision/CollisionDispatch/btInternalEdgeUtility.h
        src/Physics/BulletCollision/CollisionDispatch/btManifoldResult.cpp
        src/Physics/BulletCollision/CollisionDispatch/btManifoldResult.h
        src/Physics/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp
        src/Physics/BulletCollision/CollisionDispatch/btSimulationIslandManager.h
        src/Physics/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp
        src/Physics/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.h
        src/Physics/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp
        src/Physics/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.h
        src/Physics/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp
        src/Physics/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.h
        src/Physics/BulletCollision/CollisionDispatch/btUnionFind.cpp
        src/Physics/BulletCollision/CollisionDispatch/btUnionFind.h
        src/Physics/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp
        src/Physics/BulletCollision/CollisionDispatch/SphereTriangleDetector.h

        src/Physics/BulletCollision/CollisionShapes/btBox2dShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btBox2dShape.h
        src/Physics/BulletCollision/CollisionShapes/btBoxShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btBoxShape.h
        src/Physics/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h
        src/Physics/BulletCollision/CollisionShapes/btCapsuleShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btCapsuleShape.h
        src/Physics/BulletCollision/CollisionShapes/btCollisionMargin.h
        src/Physics/BulletCollision/CollisionShapes/btCollisionShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btCollisionShape.h
        src/Physics/BulletCollision/CollisionShapes/btCompoundShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btCompoundShape.h
        src/Physics/BulletCollision/CollisionShapes/btConcaveShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btConcaveShape.h
        src/Physics/BulletCollision/CollisionShapes/btConeShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btConeShape.h
        src/Physics/BulletCollision/CollisionShapes/btConvex2dShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btConvex2dShape.h
        src/Physics/BulletCollision/CollisionShapes/btConvexHullShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btConvexHullShape.h
        src/Physics/BulletCollision/CollisionShapes/btConvexInternalShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btConvexInternalShape.h
        src/Physics/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btConvexPointCloudShape.h
        src/Physics/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp
        src/Physics/BulletCollision/CollisionShapes/btConvexPolyhedron.h
        src/Physics/BulletCollision/CollisionShapes/btConvexShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btConvexShape.h
        src/Physics/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h
        src/Physics/BulletCollision/CollisionShapes/btCylinderShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btCylinderShape.h
        src/Physics/BulletCollision/CollisionShapes/btEmptyShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btEmptyShape.h
        src/Physics/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h
        src/Physics/BulletCollision/CollisionShapes/btMaterial.h
        src/Physics/BulletCollision/CollisionShapes/btMiniSDF.cpp
        src/Physics/BulletCollision/CollisionShapes/btMiniSDF.h
        src/Physics/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btMinkowskiSumShape.h
        src/Physics/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.h
        src/Physics/BulletCollision/CollisionShapes/btMultiSphereShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btMultiSphereShape.h
        src/Physics/BulletCollision/CollisionShapes/btOptimizedBvh.cpp
        src/Physics/BulletCollision/CollisionShapes/btOptimizedBvh.h
        src/Physics/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btPolyhedralConvexShape.h
        src/Physics/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h
        src/Physics/BulletCollision/CollisionShapes/btSdfCollisionShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btSdfCollisionShape.h
        src/Physics/BulletCollision/CollisionShapes/btShapeHull.cpp
        src/Physics/BulletCollision/CollisionShapes/btShapeHull.h
        src/Physics/BulletCollision/CollisionShapes/btSphereShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btSphereShape.h
        src/Physics/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btStaticPlaneShape.h
        src/Physics/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp
        src/Physics/BulletCollision/CollisionShapes/btStridingMeshInterface.h
        src/Physics/BulletCollision/CollisionShapes/btTetrahedronShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btTetrahedronShape.h
        src/Physics/BulletCollision/CollisionShapes/btTriangleBuffer.cpp
        src/Physics/BulletCollision/CollisionShapes/btTriangleBuffer.h
        src/Physics/BulletCollision/CollisionShapes/btTriangleCallback.cpp
        src/Physics/BulletCollision/CollisionShapes/btTriangleCallback.h
        src/Physics/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp
        src/Physics/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h
        src/Physics/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp
        src/Physics/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.h
        src/Physics/BulletCollision/CollisionShapes/btTriangleInfoMap.h
        src/Physics/BulletCollision/CollisionShapes/btTriangleMesh.cpp
        src/Physics/BulletCollision/CollisionShapes/btTriangleMesh.h
        src/Physics/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btTriangleMeshShape.h
        src/Physics/BulletCollision/CollisionShapes/btTriangleShape.h
        src/Physics/BulletCollision/CollisionShapes/btUniformScalingShape.cpp
        src/Physics/BulletCollision/CollisionShapes/btUniformScalingShape.h

        src/Physics/BulletCollision/NarrowPhaseCollision/btComputeGjkEpaPenetration.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp
        src/Physics/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp
        src/Physics/BulletCollision/NarrowPhaseCollision/btConvexCast.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btConvexPenetrationDepthSolver.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btDiscreteCollisionDetectorInterface.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btGjkCollisionDescription.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp
        src/Physics/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp
        src/Physics/BulletCollision/NarrowPhaseCollision/btGjkEpa2.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btGjkEpa3.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp
        src/Physics/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp
        src/Physics/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btManifoldPoint.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp
        src/Physics/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btMprPenetration.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp
        src/Physics/BulletCollision/NarrowPhaseCollision/btPersistentManifold.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btPointCollector.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp
        src/Physics/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp
        src/Physics/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp
        src/Physics/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btSimplexSolverInterface.h
        src/Physics/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp
        src/Physics/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.h


        src/Physics/BulletDynamics/ConstraintSolver/btBatchedConstraints.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btBatchedConstraints.h
        src/Physics/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btConeTwistConstraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btConstraintSolver.h
        src/Physics/BulletDynamics/ConstraintSolver/btContactConstraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btContactConstraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btContactSolverInfo.h
        src/Physics/BulletDynamics/ConstraintSolver/btFixedConstraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btFixedConstraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btGearConstraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btGearConstraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btGeneric6DofSpring2Constraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btGeneric6DofSpring2Constraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btHinge2Constraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btHingeConstraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btJacobianEntry.h
        src/Physics/BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.h
        src/Physics/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h
        src/Physics/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h
        src/Physics/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btSliderConstraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btSolverBody.h
        src/Physics/BulletDynamics/ConstraintSolver/btSolverConstraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btTypedConstraint.h
        src/Physics/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp
        src/Physics/BulletDynamics/ConstraintSolver/btUniversalConstraint.h

        src/Physics/BulletDynamics/Dynamics/btActionInterface.h
        src/Physics/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp
        src/Physics/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h
        src/Physics/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.cpp
        src/Physics/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h
        src/Physics/BulletDynamics/Dynamics/btDynamicsWorld.h
        src/Physics/BulletDynamics/Dynamics/btRigidBody.cpp
        src/Physics/BulletDynamics/Dynamics/btRigidBody.h
        src/Physics/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp
        src/Physics/BulletDynamics/Dynamics/btSimpleDynamicsWorld.h
        src/Physics/BulletDynamics/Dynamics/btSimulationIslandManagerMt.cpp
        src/Physics/BulletDynamics/Dynamics/btSimulationIslandManagerMt.h

        src/Physics/LinearMath/TaskScheduler/btTaskScheduler.cpp
        src/Physics/LinearMath/TaskScheduler/btThreadSupportInterface.h
        src/Physics/LinearMath/TaskScheduler/btThreadSupportPosix.cpp
        src/Physics/LinearMath/TaskScheduler/btThreadSupportWin32.cpp

        src/Physics/LinearMath/btAabbUtil2.h
        src/Physics/LinearMath/btAlignedAllocator.cpp
        src/Physics/LinearMath/btAlignedAllocator.h
        src/Physics/LinearMath/btAlignedObjectArray.h
        src/Physics/LinearMath/btConvexHull.cpp
        src/Physics/LinearMath/btConvexHull.h
        src/Physics/LinearMath/btConvexHullComputer.cpp
        src/Physics/LinearMath/btConvexHullComputer.h
        src/Physics/LinearMath/btCpuFeatureUtility.h
        src/Physics/LinearMath/btDefaultMotionState.h
        src/Physics/LinearMath/btGeometryUtil.cpp
        src/Physics/LinearMath/btGeometryUtil.h
        src/Physics/LinearMath/btGrahamScan2dConvexHull.h
        src/Physics/LinearMath/btHashMap.h
        src/Physics/LinearMath/btIDebugDraw.h
        src/Physics/LinearMath/btImplicitQRSVD.h
        src/Physics/LinearMath/btList.h
        src/Physics/LinearMath/btMatrix3x3.h
        src/Physics/LinearMath/btMatrixX.h
        src/Physics/LinearMath/btMinMax.h
        src/Physics/LinearMath/btModifiedGramSchmidt.h
        src/Physics/LinearMath/btMotionState.h
        src/Physics/LinearMath/btPolarDecomposition.cpp
        src/Physics/LinearMath/btPolarDecomposition.h
        src/Physics/LinearMath/btPoolAllocator.h
        src/Physics/LinearMath/btQuadWord.h
        src/Physics/LinearMath/btQuaternion.h
        src/Physics/LinearMath/btQuickprof.cpp
        src/Physics/LinearMath/btQuickprof.h
        src/Physics/LinearMath/btRandom.h
        src/Physics/LinearMath/btReducedVector.cpp
        src/Physics/LinearMath/btReducedVector.h
        src/Physics/LinearMath/btScalar.h
        src/Physics/LinearMath/btSerializer.cpp
        src/Physics/LinearMath/btSerializer.h
        src/Physics/LinearMath/btSerializer64.cpp
        src/Physics/LinearMath/btSpatialAlgebra.h
        src/Physics/LinearMath/btStackAlloc.h
        src/Physics/LinearMath/btThreads.cpp
        src/Physics/LinearMath/btThreads.h
        src/Physics/LinearMath/btTransform.h
        src/Physics/LinearMath/btTransformUtil.h
        src/Physics/LinearMath/btVector3.cpp
        src/Physics/LinearMath/btVector3.h

)

target_include_directories(Black_Engine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external/ImGuizmo
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
)
# stb.cpp için statik kütüphane oluştur
add_library(stb STATIC external/stb/stb.cpp)

target_link_libraries(Black_Engine PRIVATE
        glad
        imgui
        glfw
        opengl32
        ImGuizmo
        stb
)





if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(external/googletest)
    add_subdirectory(tests)
endif()