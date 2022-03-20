#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/lab_camera.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void Generate(int mat[21][21], int x1, int y1, int x2, int y2);
        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1));

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

    protected:
        glm::vec3 lightPosition;
        unsigned int materialShininess;
        float materialKd;
        float materialKs;

        implemented::Camera_Tema* camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;
        float fov = RADIANS(60);
        float width = 50.0f;
        float height = 40.0f;
        int mat[21][21];
        int x_start = 2;
        int y_start = 1;
        int x_end = 18;
        int y_end = 19;

        float time_passed;
        bool write_time;
        std::vector<int> val_x;
        std::vector<int> val_y;
        int idx;

        float x, z;
        float x_pos;
        float y_pos;
        float z_pos;
        float x_l;
        float y_l;
        float z_l;
        float x_r;
        float y_r;
        float z_r;
        float translateZ;

        bool first_person;
        bool third_person;
        bool set_first = false;
        bool set_third = true; 
        glm::vec3 fp;

        bool go_up, go_right, go_down, go_left;
        float x_enemy, z_enemy;
        float x_corp, x_inamic, z_corp, z_inamic;
        float dist;

        float collide;
        bool shoot;
        float bullet_speed;
        float dir_x;
        float dir_y;
        float dir_z;
        
        bool valid_w;
        float move_w;
        bool valid_a;
        float move_a;
        bool valid_s;
        float move_s;
        bool valid_d;
        float move_d;

        float health;
        bool finish;
        int total_time;

    };
}   // namespace m1
