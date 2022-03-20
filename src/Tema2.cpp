#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    Generate(mat, x_start, y_start, x_end, y_end); // generare labirint
    
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 21; j++) {
            if (mat[i][j] == 0) {
                val_x.push_back(i);
                val_y.push_back(j);
            }
        }
    }
    idx = rand() % val_x.size();

    cout << "aleg index poz:" << idx << endl;
    x = val_x[idx];
    z = val_y[idx];
    x_pos = x + 0.5f;
    y_pos = 0.5f;
    z_pos = z + 0.5f;
    x_l = x;
    y_l = 0.15f;
    z_l = z;
    bullet_speed = 0;
    renderCameraTarget = false;
    x_r = x;
    y_r = 0.15f;
    z_r = z;
    fp = glm::vec3(0, 0.1f, 0.1f);
    camera = new implemented::Camera_Tema();
    third_person = true;
    first_person = false;
    
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Shader* shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
     
    {
        lightPosition = glm::vec3(10, 2, 10);
        materialShininess = 30;
        materialKd = 0.5;
        materialKs = 0.5;
    }

    go_up = true;
    go_right = false;
    go_down = false;
    go_left = false;
    x_enemy = 0;
    z_enemy = 0;
    collide = 0.8f;
    health = 15;
    finish = false;
    total_time = 90;
    shoot = false;
}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    if (!finish && time_passed < total_time) {
        time_passed = glfwGetTime();

        if ((int)time_passed % 2 == 0 && write_time) {
            
            cout << "Timp ramas: " << total_time - (int)time_passed << endl;
            cout << "Viata ramasa: " << health << endl;

            if ((int)time_passed == total_time) {
                cout << "Ai pierdut!";
            } 
            write_time = false;

        }
        if ((int)time_passed % 2 == 1) {
            write_time = true;
        }

        if ((int)(x_pos - 0.5f) >= 19 && (int)(z_pos - 0.5f) >= 19) {
            finish = true;
            cout << "Ai castigat!";
        }

        x_corp = x_pos + 0.17f;
        z_corp = z_pos + 0.17f;
        dist = 2.0f;
        if (third_person) {
            if (set_third) {
                camera->Set(glm::vec3(x_pos, y_pos + 4.4f, z_pos + 3.5f), glm::vec3(x_pos, y_pos, z_pos), glm::vec3(0, 1, 0));
            }
            set_third = false;
        }
        else {
            if (set_first) {
                camera->Set(glm::vec3(x_pos, y_pos, z_pos) + fp, glm::vec3(x_pos, y_pos, z_pos), glm::vec3(0, 1, 0));
            }
            set_first = false;
        }
        //desenare labirint
        int i, j;
        for (i = 0; i < 21; i++) {
            for (j = 0; j < 21; j++) {
                if (mat[i][j] == 1) {
                    glm::mat4 modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(i + 0.5f, 1, j + 0.5f));
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 2, 1));
                    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.6f, 0.7f, 0.9f));
                } // deplasare inamici
                if (mat[i][j] == 2) {
                    if (go_up) {
                        x_enemy += 0.025 * deltaTimeSeconds;
                        if (x_enemy > 0.22f) {
                            go_up = false;
                            go_right = true;
                        }
                    }
                    if (go_right) {
                        z_enemy += 0.025 * deltaTimeSeconds;
                        if (z_enemy > 0.22f) {
                            go_right = false;
                            go_down = true;
                        }
                    }
                    if (go_down) {
                        x_enemy -= 0.025 * deltaTimeSeconds;
                        if (x_enemy < -0.22f) {
                            go_down = false;
                            go_left = true;
                        }
                    }
                    if (go_left) {
                        z_enemy -= 0.025 * deltaTimeSeconds;
                        if (z_enemy < -0.22f) {
                            go_left = false;
                            go_up = true;
                        }
                    }
                    x_corp = x_pos + 0.17f;
                    z_corp = z_pos + 0.17f;
                    x_inamic = i + 0.5f + x_enemy + 0.25f;
                    z_inamic = j + 0.5f + z_enemy + 0.25f;
                    float dist = sqrt((x_corp - x_inamic) * (x_corp - x_inamic) + (z_corp - z_inamic) * (z_corp - z_inamic));
                    if (dist > 0.33f * 1.41f / 2 + 0.5f * 1.41f / 2) { //coliziune jucator - inamic
                        glm::mat4 modelMatrix = glm::mat4(1);
                        modelMatrix = glm::translate(modelMatrix, glm::vec3(i + 0.5f + x_enemy, 0.5f, j + 0.5f + z_enemy));
                        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.7f, 0.5f));
                        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(1, 0, 0.2f));
                    }
                    else {
                        mat[i][j] = -1;
                        health -= 1.0f;
                        
                        cout << "Timp ramas: " << total_time - (int)time_passed << endl;
                        cout << "Viata ramasa: " << health << endl;

                        if (health < 1) {
                            finish = true;
                            cout << "Ai pierdut!";
                        }

                    }

                }
            }
        }

        for (i = 0; i < 21; i++) {
            for (j = 0; j < 21; j++) {
                if (mat[i][j] == 2) { //coliziuni proiectil - inamic
                    float x_proj = x_pos + bullet_speed * dir_x;
                    float z_proj = z_pos + bullet_speed * dir_z;
                    x_inamic = i + 0.5f + x_enemy + 0.25f;
                    z_inamic = j + 0.5f + z_enemy + 0.25f;
                    float dist = sqrt((x_proj - x_inamic) * (x_proj - x_inamic) + (z_proj - z_inamic) * (z_proj - z_inamic));
                    if (dist < 0.5f * 1.41f / 2 + 0.1f * 1.41f / 2 + 0.2f) {
                        mat[i][j] = -1;
                        shoot = false;
                        bullet_speed = 0;
                    }
                }
            }
        }

        //validare coliziuni jucator - perete
        {
            valid_w = mat[(int)x_pos][(int)(z_pos + collide - 1)] != 1;
            valid_a = mat[(int)(x_pos + collide - 1)][(int)z_pos] != 1;
            valid_s = mat[(int)x_pos][(int)(z_pos - collide + 1)] != 1;
            valid_d = mat[(int)(x_pos - collide + 1)][(int)z_pos] != 1;
        }
        //desenare caracter
        if (third_person) {
            {   //head
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(x_pos, y_pos + 0.4f, z_pos));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
                RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, glm::vec3(0.9f, 1.0f, 0));
            }

            {   
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(x_l + 0.5f, y_l, z_l + 0.4f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.25f, 0.1f));
                RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0, 0, 0.5f));
            }

            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(x_r + 0.5f, y_r, z_r + 0.6f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.25f, 0.1f));
                RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0, 0, 0.5f));
            }

            {   //body
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(x_pos, y_pos, z_pos));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.5f, 0.3f));
                RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.7f, 0.0f, 0.8f));
            }
        }
        if (!third_person && shoot) {
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(x_pos + bullet_speed * dir_x, y_pos + bullet_speed * dir_y, z_pos + bullet_speed * dir_z));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
                RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(1.0f, 0.0f, 0.7f));
            }
            bullet_speed += 0.05f;
            float max_dist = 4;
            if (max_dist < bullet_speed) {
               shoot = false;
               bullet_speed = 0;
            }
        }
        // desenare podea
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(10, 0.01f, 10));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
            RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, glm::vec3(0.6f, 1.0f, 0.4f));
        }
    } 
}

void Tema2::Generate(int mat[21][21], int x_start, int y_start, int x_end, int y_end) {
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 21; j++) {
            mat[i][j] = 1;
        }
    }
    srand(time(nullptr));
    cout << x_start << " " << y_start << " " << x_end << " " << y_end << "\n";
    mat[x_start][y_start] = 0;
    mat[x_end][y_end] = 0;

    int n = max(x_start, x_end) + 1;
    int m = max(y_start, y_end) + 1;

    cout << n << " " << m << "\n";

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cout << mat[i][j] << " ";
        }
        cout << "\n";
    }

    int down = x_end - x_start;
    int right = y_end - y_start;
    int dist = down + right;

    int i = x_start;
    int j = y_start;
    while (dist > 0) {
        int ok = rand() % 2;
        if (ok == 0 && i < x_end) {
            mat[++i][j] = 0;
            dist--;
        }
        if (ok == 1 && j < y_end) {
            mat[i][++j] = 0;
            dist--;
        }
    }

    cout << down << " " << right << "\n";

    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            cout << mat[i][j] << " ";
        }
        cout << "\n";
    }
    dist = right + down;

    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            if (mat[i][j] == 0) {
                int i_copy = i;
                int j_copy = j;
                int sel = rand() % 2;
                if (sel) {
                    int dir = rand() % 4;

                    if (dir == 0) { //stanga
                        if (j > 0) {
                            int len = rand() % j;
                            cout << "la stanga: " << len << endl;
                            while (len) {
                                mat[i][j--] = 0;
                                len--;
                            }
                            j = j_copy;
                        }
                    }
                    if (dir == 1) { //dreapta
                        if (j < m) {
                            int len = rand() % (m - j);
                            cout << "la dreapta: " << len << endl;
                            while (len) {
                                mat[i][j++] = 0;
                                len--;
                            }
                            j = j_copy;
                        }
                    }

                    if (dir == 2) { //sus
                        if (i > 0) {
                            int len = rand() % i;
                            cout << "la sus: " << len << endl;
                            while (len) {
                                mat[i--][j] = 0;
                                len--;
                            }
                            i = i_copy;
                        }
                    }

                    if (dir == 3) { //jos
                        if (i < n) {
                            int len = rand() % (n - i);
                            cout << "la jos: " << len << endl;
                            while (len) {
                                mat[i++][j] = 0;
                                len--;
                            }
                            i = i_copy;
                        }
                    }
                }
            }

        }
    }

    int enemies = 30;
    int sel = 0;

    int len = 0;
    int ram_down = 3;
    sel = 0;
    while (ram_down > 0) {
        for (i = 0; i < n - 1; i++) {
            for (j = 0; j < m; j++) {
                if (mat[i][j] == 0) {
                    sel = rand() % 2;
                    if (sel == 1 && ram_down > 0 && mat[i + 1][j] != 0) {
                        len = n - 1 - i;
                        while (len) {
                            mat[len][j] = 0;
                            len--;
                        }
                        ram_down--;
                    }
                }
            }
        }
        ram_down--;
    }

    len = 0;
    int ram_left = 3;
    sel = 0;
    while (ram_left > 0) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < m - 1; j++) {
                if (mat[i][j] == 0) {
                    sel = rand() % 2;
                    if (sel == 1 && ram_left > 0 && mat[i][j + 1] != 0) {
                        len = j;
                        while (len) {
                            mat[i][len] = 0;
                            len--;
                        }
                        ram_left--;
                    }
                }
            }
        }
        ram_left--;
    }

    while (enemies > 0) {
        for (i = 1; i < n - 1; i++) {
            for (j = 1; j < m - 1; j++) {
                if (mat[i][j] == 0) {
                    sel = rand() % 2;
                    if (sel && enemies > 0 && mat[i - 1][j] != 2 && mat[i][j - 1] != 2 && mat[i][j + 1] != 2 && mat[i + 1][j] != 2) {
                        mat[i][j] = 2;
                        enemies--;
                    }
                }
            }
        }
        enemies--;
    }

    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            cout << mat[i][j] << " ";
        }
        cout << "\n";
    }

    for (j = 0; j < m; j++) {
        mat[n][j] = 1;
    }
    for (i = 0; i < n; i++) {
        mat[i][m] = 1;
    }
    mat[n - 1][m] = 0;
    mat[n][m] = 0;
    cout << "\n";
    for (i = 0; i < n + 1; i++) {
        for (j = 0; j < m + 1; j++) {
            cout << mat[i][j] << " ";
        }
        cout << "\n";
    }
}

void Tema2::FrameEnd()
{
    //DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light & material properties
    // TODO(student): Set light position uniform
    GLint locLightPos = glGetUniformLocation(shader->program, "light_position");
    glUniform3fv(locLightPos, 1, glm::value_ptr(lightPosition));

    glm::vec3 eyePosition = camera->position;
    // TODO(student): Set eye position (camera position) uniform
    GLint locEyePos = glGetUniformLocation(shader->program, "eye_position");
    glUniform3fv(locEyePos, 1, glm::value_ptr(eyePosition));

    // TODO(student): Set material property uniforms (shininess, kd, ks, object color)
    GLint locMaterial = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(locMaterial, materialShininess);

    GLint locMaterialKd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(locMaterialKd, materialKd);

    GLint locMaterialKs = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(locMaterialKs, materialKs);

    GLint locObject = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(locObject, 1, glm::value_ptr(color));

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    //glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    //glm::mat4 projectionMatrix = projectionMatrix;
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{    
        float cameraSpeed = 1.0f;
        if (window->KeyHold(GLFW_KEY_W)) {
            if (valid_w) {
                z_pos -= deltaTime;
                z_l -= deltaTime;
                z_r -= deltaTime;
                camera->MoveForward(cameraSpeed * deltaTime);
            }
        }
        if (window->KeyHold(GLFW_KEY_S)) {
            if (valid_s) {
                 z_pos += deltaTime;
                 z_l += deltaTime;
                 z_r += deltaTime;
                 camera->MoveForward(-cameraSpeed * deltaTime);
            }
        }
        if (window->KeyHold(GLFW_KEY_A)) {
            if(valid_a)  {
                x_pos -= deltaTime;
                x_l -= deltaTime;
                x_r -= deltaTime;
                camera->TranslateRight(-cameraSpeed * deltaTime);
            }
        }
        if (window->KeyHold(GLFW_KEY_D)) {
            if(valid_d) {
                x_pos += deltaTime;
                x_l += deltaTime;
                x_r += deltaTime;
                camera->TranslateRight(cameraSpeed * deltaTime);
            }
        }
}

void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_SPACE) {
        shoot = true;
        dir_x = camera->forward[0];
        dir_y = camera->forward[1];
        dir_z = camera->forward[2];
    }
    if (key == GLFW_KEY_LEFT_CONTROL)
    {
        if (third_person) {
            set_first = true;
            third_person = false;
        }
        else {
            set_third = true;
            third_person = true;
        }
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            camera->RotateFirstPerson_OX(-sensivityOX * deltaY);
            camera->RotateFirstPerson_OY(-sensivityOY * deltaX);
        }

        if (window->GetSpecialKeyState() && GLFW_MOD_CONTROL) {
            renderCameraTarget = true;
            camera->RotateThirdPerson_OX(-sensivityOX * deltaY);
            camera->RotateThirdPerson_OY(-sensivityOY * deltaX);
        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
