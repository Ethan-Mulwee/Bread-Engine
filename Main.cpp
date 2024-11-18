#include <iostream>
#include <imgui/imgui.h>
#include "bMath/bMath.hpp"
#include "bMath/ext/raylib.hpp"
#include "bMath/ext/iostream.hpp"
#include "bEngine/world.hpp"

Camera camera;
const int axisLength = 4;

struct Block {
    bEngine::RigidBody* body;
    Model model;

    void render() {
        model.transform = toRay(body->getTransform());
        DrawModel(model, Vector3{0,0,0}, 1, WHITE);
    }
};

int main() {
    //Raylib stuff
    InitWindow(500,500,"test");
    SetTargetFPS(60);

    camera.position = Vector3{5,5,-5};
    camera.target = Vector3{0,0,0};
    camera.up = Vector3{0,1,0};
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;

    bEngine::World world;

    bEngine::RigidBody* body = new bEngine::RigidBody();
    body->inverseMass = (1/2.0f);
    body->inverseInertiaTensor = bMath::inverse(bMath::InertiaTensorCuboid(2,1,1,1));

    world.bodies.push_back(body);

    Block block; 
    block.model = LoadModelFromMesh(GenMeshCube(1,1,1));
    block.body = body;

    bMath::float3 bodyPoint;
    bMath::float3 dragPoint;
    bool dragging = false;

    bMath::float2 mouseDeltaPos;

    while(!WindowShouldClose()) {
        mouseDeltaPos = toBread(GetMouseDelta());

        Ray screenRay = GetScreenToWorldRay(GetMousePosition(), camera);
        RayCollision collision = GetRayCollisionMesh(screenRay, block.model.meshes[0], block.model.transform);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (collision.hit) {
                bodyPoint = block.body->positionToBodySpace((toBread(collision.point)));
                dragPoint = toBread(collision.point);
                dragging = true;
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            dragging = false;
        }


        if (dragging) {
            bMath::float3 viewVector = normalize(toBread(camera.target) - toBread(camera.position));
            bMath::float3 tagentViewVector = normalize(bMath::cross(toBread(camera.up), viewVector));
            bMath::float3 cameraUpVector = bMath::cross(viewVector, tagentViewVector);
            bMath::Matrix3 spaceRotation(
              tagentViewVector.x, cameraUpVector.x, viewVector.x,
              tagentViewVector.y, cameraUpVector.y, viewVector.y,
              tagentViewVector.z, cameraUpVector.z, viewVector.z
            );
            dragPoint += bMath::float3(-mouseDeltaPos.x*0.005, -mouseDeltaPos.y*0.005, 0)*spaceRotation;
            bMath::float3 worldSpaceBodyPoint = bodyPoint*body->getTransform();
            bMath::float3 force = dragPoint - worldSpaceBodyPoint;
            block.body->addForceAtPoint(force, worldSpaceBodyPoint);
        }


        world.step(1/60.0f);

        UpdateCamera(&camera, CAMERA_ORBITAL);
        BeginDrawing();
            ClearBackground(Color{35,35,35,255});
            BeginMode3D(camera);
                DrawGrid(5,1);
                DrawLine3D(Vector3{-axisLength,0,0}, Vector3{axisLength,0,0}, RED);
                DrawLine3D(Vector3{0,-axisLength,0}, Vector3{0,axisLength+0.5,0}, GREEN);
                DrawLine3D(Vector3{0,0,-axisLength}, Vector3{0,0,axisLength}, BLUE);

                block.render();
                if (collision.hit)
                    DrawSphere(collision.point, 0.1, PURPLE);
                if (dragging) {
                    DrawSphere(toRay(bodyPoint*body->getTransform()), 0.1,RED);
                    DrawSphere(toRay(dragPoint), 0.1, ORANGE);
                    DrawLine3D(toRay(bodyPoint*body->getTransform()), toRay(dragPoint), BLACK);
                }
            EndMode3D();
        EndDrawing();
    }
}