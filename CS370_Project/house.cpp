// CS370 Final Project
// Fall 2022
//Nathan Dow

#define STB_IMAGE_IMPLEMENTATION
#include "../common/stb_image.h"	// Sean Barrett's image loader - http://nothings.org/
#include <stdio.h>
#include <vector>
#include "../common/vgl.h"
#include "../common/objloader.h"
#include "../common/utils.h"
#include "../common/vmath.h"
#include "lighting.h"
#include "../CS370_Assign04/MatNode.h"
#include "../CS370_Assign04/TexNode.h"

#define DEG2RAD (M_PI/180.0)

using namespace vmath;
using namespace std;

// Vertex array and buffer names
enum VAO_IDs {Cube, Chair, Table, Sphere,Teapot,Mug,foxstatue,cylinder, cone, octahedron, circle,jet,Mirror,Frame,FoxCube, WindowCube,lightoncube,lightoffcube, NumVAOs};
enum ObjBuffer_IDs {PosBuffer, NormBuffer, TexBuffer, NumObjBuffers};
enum Color_Buffer_IDs {RedCube,SphereOrange, BlueCube,YellowChair,GreenCube,CyanCube, PurpleTable,JetBlack, NumColorBuffers};
enum LightBuffer_IDs {LightBuffer, NumLightBuffers};
enum MaterialBuffer_IDs {MaterialBuffer, NumMaterialBuffers};
enum MaterialNames {CyanPlastic, YellowAcrylic, GreenAcrylic,RedAcrylic,CyanAcrylic, Wood, WhiteBlind,YellowStar,Black};
enum Textures {Blank, Floor, Carpet , on, off,fox,brick,Window, Tree, Skirt,MirrorTex ,NumTextures};

// Vertex array and buffer objects
GLuint VAOs[NumVAOs];
GLuint ObjBuffers[NumVAOs][NumObjBuffers];
GLuint ColorBuffers[NumColorBuffers];
GLuint LightBuffers[NumLightBuffers];
GLuint MaterialBuffers[NumMaterialBuffers];
GLuint TextureIDs[NumTextures]; //NumTetures

// Number of vertices in each object
GLint numVertices[NumVAOs];

// Number of component coordinates
GLint posCoords = 4;
GLint normCoords = 3;
GLint texCoords = 2;
GLint colCoords = 4;

// Model files
vector<const char *> objFiles = {"../models/unitcube.obj", "../models/Chair.obj", "../models/Table.obj", "../models/Sphere.obj","../models/Teapot.obj","../models/Mug.obj","../models/foxstatue.obj","../models/cylinder.obj", "../models/cone.obj", "../models/octahedron.obj", "../models/circle.obj","../models/jet.obj","../models/plane.obj"};

// Texture files
vector<const char *> texFiles = {"../textures/blank.png","../textures/floor.jpg", "../textures/carpet.jpg", "../textures/on.png","../textures/off.png",   "../textures/fox.jpeg", "../textures/brick.png", "../textures/Window.png","../textures/tree.png", "../textures/skirt.jpg" };

// Camera
vec3 eye = {-3.0f, 1.0f, 0.0f};
vec3 center = {0.0f, 1.0f, 0.0f};
vec3 up = {0.0f, 1.0f, 0.0f};

vec3 mirror_eye = {0.0f, 1.0f, -5.0f};
vec3 mirror_center = {0.0f, 1.0f, 0.0f};
vec3 mirror_up = {0.0f, 1.0f, 0.0f};

GLfloat azimuth = 0.0f;
GLfloat daz = 2.0f;
GLfloat elevation = 90.0f;
GLfloat del = 2.0f;
#define BASE_WIDTH 0.25f
#define BASE_HEIGHT 0.375f
#define BASE_DEPTH 0.25f
GLfloat jetY = 1.5f;
#define TREE_WIDTH .75f
#define TREE_HEIGHT 1.0f
#define TREE_DEPTH .75f

#define STAR_WIDTH 0.25f
#define STAR_HEIGHT 0.25f
#define STAR_DEPTH 0.25f

#define TREE_X 4.0f
#define TREE_Y 0.0f
#define TREE_Z 4.0f

#define SKIRT_WIDTH 1.0f
#define SKIRT_HEIGHT 0.5f
#define SKIRT_DEPTH 1.0f
GLfloat radius = 2.0f;
GLboolean pouring = false;
GLfloat blindAng = 0.0f;
GLfloat dr = 0.1f;
GLfloat tree_theta = 0.0f;
GLfloat treeRPM = 2.5f;
GLfloat blind_dir = 1.0f;
GLfloat min_radius = 2.0f;
GLdouble rpm = 10.0;
GLfloat dir = 0.0f;
GLfloat pot_theta = 20.0f;
GLfloat fan_dir = 1;
GLfloat blindNum = 1.0f;
GLboolean blindRotate = false;
GLboolean tree_spin = true;
GLdouble elTime = 0.0;
int lightswitchon = 1;
GLboolean fan_spin = true;
GLboolean jet_fly = true;
GLfloat fan_angle = 0.0;
GLfloat fox_angle = 0.0;
GLfloat jet_angle = 0.0;
GLfloat fan_dps = 360.0f;
GLfloat jet_radius = 4.0f;
GLfloat jet_height = 2.0f;
GLfloat ppm = 6.0;
GLboolean tipping = false;
GLint pour_dir = -1.0;
GLfloat pour_ang = -30.0f;
// Shader variables
// Default (color) shader program references
GLuint default_program;
GLuint default_vPos;
GLuint default_vCol;
GLuint default_proj_mat_loc;
GLuint default_cam_mat_loc;
GLuint default_model_mat_loc;
const char *default_vertex_shader = "../default.vert";
const char *default_frag_shader = "../default.frag";
GLuint debug_mirror_program;
const char *debug_mirror_vertex_shader = "../debugMirror.vert";
const char *debug_mirror_frag_shader = "../debugMirror.frag";
// Lighting shader program reference
GLuint lighting_program;
GLuint lighting_vPos;
GLuint lighting_vNorm;
GLuint lighting_camera_mat_loc;
GLuint lighting_model_mat_loc;
GLuint lighting_proj_mat_loc;
GLuint lighting_norm_mat_loc;
GLuint lighting_lights_block_idx;
GLuint lighting_materials_block_idx;
GLuint lighting_material_loc;
GLuint lighting_num_lights_loc;
GLuint lighting_light_on_loc;
GLfloat potdir = 1.0f;
GLuint lighting_eye_loc;
const char *lighting_vertex_shader = "../lighting.vert";
const char *lighting_frag_shader = "../lighting.frag";

// Texture shader program reference
GLuint texture_program;
GLuint texture_vPos;
GLuint texture_vTex;
GLuint texture_proj_mat_loc;
GLuint texture_camera_mat_loc;
GLuint texture_model_mat_loc;
const char *texture_vertex_shader = "../texture.vert";
const char *texture_frag_shader = "../texture.frag";


GLboolean mirror = false;

// Global state
mat4 proj_matrix;
mat4 camera_matrix;
mat4 normal_matrix;
mat4 model_matrix;

vector<LightProperties> Lights;
vector<MaterialProperties> Materials;
GLuint numLights = 0;
GLint lightOn[8] = {0, 1, 1, 0, 0, 0, 0, 0};


TexNode tree;
MatNode star;
MatNode base;
TexNode skirt;

// Global screen sssssssss
GLint ww,hh;

void display();
void render_scene();
void build_geometry();
void build_solid_color_buffer(GLuint num_vertices, vec4 color, GLuint buffer);
void build_materials( );
void build_lights( );
void build_lighting_node(MatNode& node, GLuint obj, GLuint material, GLboolean transparent, mat4 base_trans);
void build_texture_node(TexNode& node, GLuint obj, GLuint texture, mat4 base_trans);
void build_fox_cube(GLuint obj);
void build_lighton_cube(GLuint obj);
void build_lightoff_cube(GLuint obj);
void build_window_cube(GLuint obj);
void build_textures();
void build_scene_graph( );
void draw_frame(GLuint obj);
void create_mirror( );
void traverse_scene_graph(BaseNode *node, mat4 baseTransform);

void build_frame(GLuint obj);
void build_mirror();
void load_object(GLuint obj);
void draw_color_obj(GLuint obj, GLuint color);
void draw_mat_object(GLuint obj, GLuint material);
void renderQuad(GLuint shader, GLuint tex);
void draw_tex_object(GLuint obj, GLuint texture);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, int button, int action, int mods);

int main(int argc, char**argv)
{
	// Create OpenGL window
	GLFWwindow* window = CreateWindow("Think Inside The Box");
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    } else {
        printf("OpenGL window successfully created\n");
    }

    // Store initial window size
    glfwGetFramebufferSize(window, &ww, &hh);

    // Register callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window,key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);

    // Load shaders and associate variables
    ShaderInfo default_shaders[] = { {GL_VERTEX_SHADER, default_vertex_shader},{GL_FRAGMENT_SHADER, default_frag_shader},{GL_NONE, NULL} };
    default_program = LoadShaders(default_shaders);
    default_vPos = glGetAttribLocation(default_program, "vPosition");
    default_vCol = glGetAttribLocation(default_program, "vColor");
    default_proj_mat_loc = glGetUniformLocation(default_program, "proj_matrix");
    default_cam_mat_loc = glGetUniformLocation(default_program, "camera_matrix");
    default_model_mat_loc = glGetUniformLocation(default_program, "model_matrix");

    // Load shaders
    // Load light shader
    ShaderInfo lighting_shaders[] = { {GL_VERTEX_SHADER, lighting_vertex_shader},{GL_FRAGMENT_SHADER, lighting_frag_shader},{GL_NONE, NULL} };
    lighting_program = LoadShaders(lighting_shaders);
    lighting_vPos = glGetAttribLocation(lighting_program, "vPosition");
    lighting_vNorm = glGetAttribLocation(lighting_program, "vNormal");
    lighting_proj_mat_loc = glGetUniformLocation(lighting_program, "proj_matrix");
    lighting_camera_mat_loc = glGetUniformLocation(lighting_program, "camera_matrix");
    lighting_norm_mat_loc = glGetUniformLocation(lighting_program, "normal_matrix");
    lighting_model_mat_loc = glGetUniformLocation(lighting_program, "model_matrix");
    lighting_lights_block_idx = glGetUniformBlockIndex(lighting_program, "LightBuffer");
    lighting_materials_block_idx = glGetUniformBlockIndex(lighting_program, "MaterialBuffer");
    lighting_material_loc = glGetUniformLocation(lighting_program, "Material");
    lighting_num_lights_loc = glGetUniformLocation(lighting_program, "NumLights");
    lighting_light_on_loc = glGetUniformLocation(lighting_program, "LightOn");
    lighting_eye_loc = glGetUniformLocation(lighting_program, "EyePosition");

    // Load texture shaders
    ShaderInfo texture_shaders[] = { {GL_VERTEX_SHADER, texture_vertex_shader},{GL_FRAGMENT_SHADER, texture_frag_shader},{GL_NONE, NULL} };
    texture_program = LoadShaders(texture_shaders);
    texture_vPos = glGetAttribLocation(texture_program, "vPosition");
    texture_vTex = glGetAttribLocation(texture_program, "vTexCoord");
    texture_proj_mat_loc = glGetUniformLocation(texture_program, "proj_matrix");
    texture_camera_mat_loc = glGetUniformLocation(texture_program, "camera_matrix");
    texture_model_mat_loc = glGetUniformLocation(texture_program, "model_matrix");


    // Load debug mirror shader
    ShaderInfo debug_mirror_shaders[] = { {GL_VERTEX_SHADER, debug_mirror_vertex_shader},{GL_FRAGMENT_SHADER, debug_mirror_frag_shader},{GL_NONE, NULL} };
    debug_mirror_program = LoadShaders(debug_mirror_shaders);

    // Create geometry buffers
    build_geometry();
    // Create material buffers
    build_materials();
    build_fox_cube(FoxCube);
    build_window_cube(WindowCube);
    build_lighton_cube(lightoncube);
    build_lightoff_cube(lightoffcube);
    // Create light buffers
    build_lights();
    // Create textures
    build_textures();
    build_mirror();
build_scene_graph();
    // Enable depth test
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Set Initial camera position


    // Start wloop
    while ( !glfwWindowShouldClose( window ) ) {
    	// Draw graphics
        create_mirror();
        //renderQuad(debug_mirror_program, MirrorTex);
        display();
        // Update other events like input handling
        glfwPollEvents();




        GLdouble curTime = glfwGetTime();
        GLdouble dT = curTime - elTime;



        if(tree_spin == true)
        {
            base.set_update_transform(translate(TREE_X,0.0f,TREE_Z)*rotate(tree_theta,0.0f,1.0f,0.0f)*translate(-TREE_X,-0.5f,-TREE_Z));
            skirt.set_update_transform(translate(TREE_X,-.25f,TREE_Z)*rotate(-tree_theta,0.0f,1.0f,0.0f)*translate(-TREE_X,-0.1f,-TREE_Z));
            tree_theta = tree_theta + dT *treeRPM+.01f;
        }



        /*
        pot_theta = pot_theta + ppm*dT*pour_dir;
        if(pot_theta<=pour_ang || pot_theta >= -.50f) {

            pour_dir = pour_dir * -1;
            pouring = !pouring;
        }*/
        fox_angle += 1.0f*dT*fan_dps;
        if(jet_fly == true)
        {
            jet_angle += 1.0*dT*fan_dps;

        }
        if(fan_spin == true)
        {
            fan_angle += 1.0*dT*fan_dps;

        }
        //pot_theta = -blindAng;

        //blind test 3
        if(blindRotate == true) {
            blindRotate = false;
            blindAng = blindAng + blind_dir;
           // for (int i = 0; i < 60; i++) {
             //   blindAng = blindAng + blind_dir;
            //}
            if(blindAng == 0.0f || blindAng == 60.0f)
            {
                blind_dir = -blind_dir;
            }
           // blind_dir = -blind_dir;

        }

        if(pouring == true) {
            pouring = false;
            pot_theta = pot_theta + potdir;
            // for (int i = 0; i < 60; i++) {
            //   blindAng = blindAng + blind_dir;
            //}
            if(pot_theta == 0.0f || pot_theta == 60.0f)
            {
                potdir = -potdir;
            }
            // blind_dir = -blind_dir;

        }



        elTime = curTime;





        // Swap buffer onto screen
        glfwSwapBuffers( window );
    }

    // Close window
    glfwTerminate();
    return 0;

}
void traverse_scene_graph(BaseNode *node, mat4 baseTransform) {
    mat4 model_matrix;

    // Stop when at bottom of branch
    if (node == NULL) {
        return;
    }

    // Apply local transformation and render
    model_matrix = baseTransform*node->ModelTransform;

    node->draw(proj_matrix, camera_matrix, model_matrix);

    // Recurse vertically if possible (depth-first)
    if (node->Child != NULL) {
        traverse_scene_graph(node->Child, model_matrix);
    }

    // Remove local transformation and recurse horizontal
    if (node->Sibling != NULL) {
        traverse_scene_graph(node->Sibling, baseTransform);
    }
}
void display( )
{
    // Declare projection and camera matrices
    proj_matrix = mat4().identity();
    camera_matrix = mat4().identity();

	// Clear window and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute anisotropic scaling
    GLfloat xratio = 1.0f;
    GLfloat yratio = 1.0f;
    // If taller than wide adjust y
    if (ww <= hh)
    {
        yratio = (GLfloat)hh / (GLfloat)ww;
    }
        // If wider than tall adjust x
    else if (hh <= ww)
    {
        xratio = (GLfloat)ww / (GLfloat)hh;
    }

    // DEFAULT ORTHOGRAPHIC PROJECTION

    proj_matrix = frustum(-.10f*xratio, .10f*xratio, -.10f*yratio, .10f*yratio, .2f, 15.0f);

    // Set camera matrix
    camera_matrix = lookat(eye, center, up);

    // Render objects
	render_scene();

	// Flush pipeline
	glFlush();
}

void render_scene( ) {
    traverse_scene_graph(&base, mat4().identity());
    // Declare transformation matrices
    model_matrix = mat4().identity();
    mat4 scale_matrix = mat4().identity();
    mat4 rot_matrix = mat4().identity();
    mat4 trans_matrix = mat4().identity();
    mat4 rot2_matrix = mat4().identity();
    mat4 rot3_matrix = mat4().identity();
    mat4 rot4_matrix = mat4().identity();
    // Set cube transformation matrix


    trans_matrix = translate(0.0f, 3.4f, 0.0f);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(11.0f, 0.0f, 11.0f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    //draw_mat_object(Cube, CyanPlastic);
    draw_tex_object(Cube, Floor);
    // Draw cube roof
     //draw_color_obj(Cube, CyanCube);



    trans_matrix = translate(0.0f, 0.0f, 5.5f);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(11.0f, 7.0f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    // Draw wall 1
   // draw_mat_object(Cube, CyanPlastic);
    draw_tex_object(Cube, brick);


    trans_matrix = translate(0.0f, 0.0f, -5.5f);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(11.0f, 7.0f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    // Draw wall 2
    // draw_mat_object(Cube, CyanPlastic);
    draw_tex_object(Cube, brick);



    trans_matrix = translate(5.5f, 0.0f, 0.0f);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(0.5f, 7.0f, 11.0f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    // Draw wall 3
    // draw_mat_object(Cube, CyanPlastic);
    draw_tex_object(Cube, brick);



    trans_matrix = translate(-5.5f, 0.0f, 0.0f);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(0.5f, 7.0f, 11.0f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    // Draw wall 4
    // draw_mat_object(Cube, CyanPlastic);
    draw_tex_object(Cube, brick);





    trans_matrix = translate(-1.0f, -0.25f, 0.0f);
    rot_matrix = rotate(0.0f, vec3(0.0f, 1.0f, 0.0f));
    scale_matrix = scale(0.5f, 0.5f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    // Draw chair1 yellow
    draw_mat_object(Chair, Wood);

    trans_matrix = translate(0.0f, -0.25f, 1.0f);
    rot_matrix = rotate(90.0f, vec3(0.0f, 1.0f, 0.0f));
    scale_matrix = scale(0.5f, 0.5f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    // Draw chair2 yellow
    draw_mat_object(Chair, Wood);

    trans_matrix = translate(0.0f, -0.25f, -1.0f);
    rot_matrix = rotate(270.0f, vec3(0.0f, 1.0f, 0.0f));
    scale_matrix = scale(0.5f, 0.5f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    // Draw chair3 yellow
    draw_mat_object(Chair, Wood);


    trans_matrix = translate(1.0f, -0.25f, 0.0f);
    rot_matrix = rotate(180.0f, vec3(0.0f, 1.0f, 0.0f));
    scale_matrix = scale(0.5f, 0.5f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    // Draw chair4 yellow
    draw_mat_object(Chair, Wood);


 GLfloat temp = -cos(jet_angle);
    trans_matrix = translate(0.0f, jet_height,jet_radius);
    rot2_matrix = rotate(-90.0f,vec3(1.0f,0.0f,-1.0f));
   rot_matrix = rotate(temp, vec3(0.0f, -01.0f, 0.0f));
    scale_matrix = scale(.001f, .001f, .001f);
    rot4_matrix = rotate(180.0f, vec3(0.0f,1.0f,0.0f));
    rot3_matrix = rotate(120.0f+ -jet_angle, vec3(0.0f,1.0f,0.0f));
    model_matrix = rot3_matrix*trans_matrix*rot_matrix*rot4_matrix*rot2_matrix*scale_matrix;
   // normal_matrix = model_matrix.inverse().transpose();
    draw_mat_object(jet, WhiteBlind);
    //draw_color_obj(jet, JetBlack);
   // draw_color_obj(Cube, RedCube);



    trans_matrix = translate(0.0f, 3.0f, 0.0f);
    rot_matrix = rotate(0.0f +fan_angle, vec3(0.0f, 01.0f, 0.0f));
    scale_matrix = scale(0.5f, 0.10f, 04.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    // Draw cube Fan blade 1
    //draw_color_obj(Cube, BlueCube);
   // draw_mat_object(Cube, Wood);
        draw_mat_object(Cube, WhiteBlind);

    trans_matrix = translate(0.0f, 3.0f, 0.0f);
    rot_matrix = rotate(120.0f + fan_angle, vec3(0.0f, 01.0f, 0.0f));
    scale_matrix = scale(0.5f, 0.1f, 4.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    // Draw cube Fan bladed 2
    //draw_color_obj(Cube, BlueCube);
    // draw_mat_object(Cube, Wood);
    draw_mat_object(Cube, WhiteBlind);


    trans_matrix = translate(0.0f, 3.0f, 0.0f);
    rot_matrix = rotate(240.0f + fan_angle, vec3(0.0f, 01.0f, 0.0f));
    scale_matrix = scale(0.5f, 0.10f, 4.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    // Draw cube Fan blade 3
    //draw_color_obj(Cube, BlueCube);
    // draw_mat_object(Cube, Wood);
    draw_mat_object(Cube, WhiteBlind);



    trans_matrix = translate(5.3f, 1.0f, 0.0f);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(0.25f, 2.5f, 1.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    // Draw cube Door
    draw_mat_object(Cube, Wood);



    if(lightswitchon == 1) {
        trans_matrix = translate(5.35f, 1.0f, -1.5f);
        rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
        scale_matrix = scale(0.250f, 0.750f, 0.50f);
        model_matrix = trans_matrix * rot_matrix * scale_matrix;
        // Draw cube lightswitch ON
        //draw_tex_object(Cube, on);
        draw_tex_object(lightoncube, on);
    }

    if(lightswitchon == 0)
    {
        trans_matrix = translate(5.35f, 1.0f, -1.5f);
        rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
        scale_matrix = scale(0.250f, 0.75f, 0.50f);
        model_matrix = trans_matrix * rot_matrix * scale_matrix;
        // Draw cube lightswitch OFF
        //draw_tex_object(Cube, off);
        draw_tex_object(lightoffcube, off);
    }


    trans_matrix = translate(0.0f, 1.0f, 5.33f);
    rot_matrix = rotate(90.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(2.0f, 2.0f, .20f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    // Draw cube Window
    //draw_color_obj(Cube, GreenCube);
    draw_tex_object(WindowCube, Window);

    trans_matrix = translate(0.0f, 1.0f+blindNum, 5.20f);
    rot_matrix = rotate(-blindAng, 1.0f,0.0f,0.0f);
    scale_matrix = scale(2.0f, 0.02f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    //draw_color_obj(Cube, RedCube);
    draw_mat_object(Cube,WhiteBlind);


    trans_matrix = translate(0.0f, 0.8f+blindNum, 5.20f);
    rot_matrix = rotate(-blindAng, 1.0f,0.0f,0.0f);
    scale_matrix = scale(2.0f, 0.02f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    //draw_color_obj(Cube, RedCube);
    draw_mat_object(Cube,WhiteBlind);

    trans_matrix = translate(0.0f, 0.6f+blindNum, 5.20f);
    rot_matrix = rotate(-blindAng, 1.0f,0.0f,0.0f);
    scale_matrix = scale(2.0f, 0.02f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    //draw_color_obj(Cube, RedCube);
    draw_mat_object(Cube,WhiteBlind);

    trans_matrix = translate(0.0f, 0.4f+blindNum, 5.20f);
    rot_matrix = rotate(-blindAng, 1.0f,0.0f,0.0f);
    scale_matrix = scale(2.0f, 0.02f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    //draw_color_obj(Cube, RedCube);
    draw_mat_object(Cube,WhiteBlind);

    trans_matrix = translate(0.0f, 0.2f+blindNum, 5.20f);
    rot_matrix = rotate(-blindAng, 1.0f,0.0f,0.0f);
    scale_matrix = scale(2.0f, 0.02f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    //draw_color_obj(Cube, RedCube);
    draw_mat_object(Cube,WhiteBlind);

    trans_matrix = translate(0.0f, 0.2f+blindNum, 5.20f);
    rot_matrix = rotate(-blindAng, 1.0f,0.0f,0.0f);
    scale_matrix = scale(2.0f, 0.02f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    //draw_color_obj(Cube, RedCube);
    draw_mat_object(Cube,WhiteBlind);

    trans_matrix = translate(0.0f, 0.0f+blindNum, 5.20f);
    rot_matrix = rotate(-blindAng, 1.0f,0.0f,0.0f);
    scale_matrix = scale(2.0f, 0.02f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    //draw_color_obj(Cube, RedCube);
    draw_mat_object(Cube,WhiteBlind);

    trans_matrix = translate(0.0f, -0.2f+blindNum, 5.20f);
    rot_matrix = rotate(-blindAng, 1.0f,0.0f,0.0f);
    scale_matrix = scale(2.0f, 0.02f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    //draw_color_obj(Cube, RedCube);
    draw_mat_object(Cube,WhiteBlind);

    trans_matrix = translate(0.0f, -0.4f+blindNum, 5.20f);
    rot_matrix = rotate(-blindAng, 1.0f,0.0f,0.0f);
    scale_matrix = scale(2.0f, 0.02f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    //draw_color_obj(Cube, RedCube);
    draw_mat_object(Cube,WhiteBlind);

    trans_matrix = translate(0.0f, -0.6f+blindNum, 5.20f);
    rot_matrix = rotate(-blindAng, 1.0f,0.0f,0.0f);
    scale_matrix = scale(2.0f, 0.02f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    //draw_color_obj(Cube, RedCube);
    draw_mat_object(Cube,WhiteBlind);

    trans_matrix = translate(0.0f, -0.8f+blindNum, 5.20f);
    rot_matrix = rotate(-blindAng, 1.0f,0.0f,0.0f);
    scale_matrix = scale(2.0f, 0.02f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    //draw_color_obj(Cube, RedCube);
    draw_mat_object(Cube,WhiteBlind);








    trans_matrix = translate(-5.33f, 1.5f, 0.0f);
    rot_matrix = rotate(-90.0f, vec3(1.0f, 0.0f, 0.0f));
    scale_matrix = scale(.2f, 3.0f, 3.0f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    // Draw cube Painting
   // draw_color_obj(Cube, GreenCube);
  //  draw_tex_object(Cube, fox);
    draw_tex_object(FoxCube, fox);

    trans_matrix = translate(0.0f, -0.23f, 0.0f);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(3.0f, 0.0f, 3.0f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;

    // Draw cube Rug
    draw_tex_object(Cube, Carpet);


    /*
    trans_matrix = translate(0.0f, 1.0f, -5.2f);
    rot_matrix = rotate(90.0f, vec3(0.0f, 1.0f, 0.0f));
    scale_matrix = scale(.2f, 2.0f, 2.0f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    // Draw cube mirror
    draw_color_obj(Cube, BlueCube);
*/




    trans_matrix = translate(0.0f, -0.3f, 0.0f);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(11.0f, 0.0f, 11.0f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;

    // Draw cube Floor
    draw_tex_object(Cube, Floor);
    // draw_color_obj(Cube, RedCube);







    trans_matrix = translate(-0.3f, 0.0f, 0.15f);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(0.5f, 0.5f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    // Draw purple table
    draw_mat_object(Table, Wood);

    trans_matrix = translate(-.30f, 0.67f, -0.390f);
    scale_matrix = scale(.09f,.05f,.09f);
    model_matrix = trans_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    draw_mat_object(cylinder, Black);

    trans_matrix = translate(0.05f, 1.10f, -0.40f);
    rot2_matrix = rotate(-pot_theta, 0.0f,0.0f,-1.0f);
    scale_matrix = scale(0.2f,0.2f,0.2f);
    rot_matrix = rotate(180.0f,0.0f,1.0f,0.0f);

    model_matrix = trans_matrix*rot2_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    glDepthMask(GL_FALSE);
    draw_mat_object(Teapot, RedAcrylic);
    glDepthMask(GL_TRUE);

    trans_matrix = translate(-.30f, 0.60f, -0.390f);
    scale_matrix = scale(0.1f,0.1f,0.1f);
    model_matrix = trans_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    glDepthMask(GL_FALSE);
    draw_mat_object(Mug, GreenAcrylic);
    glDepthMask(GL_TRUE);

    trans_matrix = translate(-0.2f, 0.65f, 0.20f);
    rot_matrix = rotate(-fox_angle/3.0f, vec3(0.0f, 1.0f, 0.0f));
    scale_matrix = scale(0.20f, 0.20f, 0.20f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    // Draw fox statue
    // draw_color_obj(Cube, BlueCube);
    glDepthMask(GL_FALSE);
    draw_mat_object(foxstatue, CyanAcrylic);
    glDepthMask(GL_TRUE);


    trans_matrix = translate(0.0f, 3.0f, 0.0f);
    rot_matrix = rotate(0.0f, vec3(0.0f, 0.0f, 1.0f));
    scale_matrix = scale(0.5f, 0.5f, 0.5f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    normal_matrix = model_matrix.inverse().transpose();
    // Draw cube Fan
    // draw_color_obj(Sphere, SphereOrange);
    glDepthMask(GL_FALSE);
    draw_mat_object(Sphere, YellowAcrylic);
    glDepthMask(GL_TRUE);



    if (!mirror) {
        draw_frame(Frame);
        // Render mirror in scene
        // TODO: Set mirror translation
        trans_matrix = translate(mirror_eye);
        rot_matrix = rotate(90.0f, vec3(1.0f, 0.0f, 0.0f));
        rot2_matrix = rotate(180.0f,0.0f,1.0f,0.0f);
        scale_matrix = scale(1.0f, 2.0f, 1.0f);
        model_matrix = trans_matrix *rot_matrix * rot2_matrix* scale_matrix;
        // TODO: Draw mirror
        draw_tex_object(Mirror, MirrorTex);
    }


}
void build_mirror( ) {
    // Generate mirror texture
    glGenTextures(1, &TextureIDs[MirrorTex]);
    // Bind mirror texture
    glBindTexture(GL_TEXTURE_2D, TextureIDs[MirrorTex]);
    // TODO: Create empty mirror texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ww, hh, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void build_frame(GLuint obj) {
    vector<vec4> vertices;
    vector<vec3> normals;

    // Create wireframe for mirror
    vertices = {
            vec4(1.0f, 0.0f, -1.0f, 1.0f),
            vec4(1.0f, 0.0f, 1.0f, 1.0f),
            vec4(-1.0f, 0.0f, 1.0f, 1.0f),
            vec4(-1.0f, 0.0f, -1.0f, 1.0f)
    };

    normals = {
            vec3(0.0f, 1.0f, 0.0f),
            vec3(0.0f, 1.0f, 0.0f),
            vec3(0.0f, 1.0f, 0.0f),
            vec3(0.0f, 1.0f, 0.0f)
    };

    numVertices[obj] = vertices.size();

    // Create and load object buffers
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);
    glBindVertexArray(VAOs[obj]);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void create_mirror( ) {
    // Clear framebuffer for mirror rendering pass
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: Set mirror projection matrix
    proj_matrix = frustum(-.50f, 0.5f, -0.5f, 0.5f, .50f, 100.0f);

    // TODO: Set mirror camera matrix
    camera_matrix = lookat(mirror_eye, mirror_center, mirror_up);

    // Render mirror scene (without mirror)
    mirror = true;
    render_scene();
    glFlush();
    mirror = false;

    // TODO: Activate texture unit 0
    glActiveTexture(GL_TEXTURE0);
    // TODO: Bind mirror texture
    glBindTexture(GL_TEXTURE_2D, TextureIDs[MirrorTex]);
    // TODO: Copy framebuffer into mirror texture
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, ww, hh, 0);
    // Reset viewport
    glViewport(0, 0, ww, hh);
}
void build_lighting_node(MatNode& node, GLuint obj, GLuint material, GLboolean transparent, mat4 base_trans){
    // Set shader program and matrix references
    node.set_shader(lighting_program, lighting_proj_mat_loc, lighting_camera_mat_loc, lighting_norm_mat_loc, lighting_model_mat_loc);
    // Set object buffers
    node.set_buffers(VAOs[obj], ObjBuffers[obj][PosBuffer], lighting_vPos, posCoords, ObjBuffers[obj][NormBuffer], lighting_vNorm, normCoords, numVertices[obj]);
    // Set material buffers and material
    node.set_materials(MaterialBuffers[MaterialBuffer], lighting_materials_block_idx, Materials.size()*sizeof(MaterialProperties), lighting_material_loc, material, transparent);
    // Set light buffers
    node.set_lights(LightBuffers[LightBuffer], lighting_lights_block_idx, Lights.size()*sizeof(LightProperties), lighting_num_lights_loc, Lights.size(), lighting_light_on_loc, lightOn);
    // Set eye position
    node.set_eye(lighting_eye_loc, eye);
    // Set base transform
    node.set_base_transform(base_trans);
    // Set default update transform and nodes
    node.set_update_transform(mat4().identity());
    node.attach_nodes(NULL, NULL);
}

void build_texture_node(TexNode& node, GLuint obj, GLuint texture, mat4 base_trans){
    // Set shader program and matrix references
    node.set_shader(texture_program, texture_proj_mat_loc, texture_camera_mat_loc, texture_model_mat_loc);
    // Set object buffers
    node.set_buffers(VAOs[obj], ObjBuffers[obj][PosBuffer], texture_vPos, posCoords, ObjBuffers[obj][TexBuffer], texture_vTex, texCoords, numVertices[obj]);
    // Set texture
    node.set_texture(TextureIDs[texture]);
    // Set base transform
    node.set_base_transform(base_trans);
    // Default update transform and nodes
    node.set_update_transform(mat4().identity());
    node.attach_nodes(NULL, NULL);
}

void build_scene_graph( ) {

    // TODO: Add base node (lighting)
    build_lighting_node(base, cylinder, Wood, false, translate(TREE_X,0.5f,TREE_Z)*scale(BASE_WIDTH, BASE_HEIGHT, BASE_DEPTH));

    // TODO: Add tree node (texture)
    base.attach_nodes(&tree, &skirt);
    build_texture_node(tree, cone, Tree,translate(TREE_X,1.5f,TREE_Z)* scale(TREE_WIDTH,TREE_HEIGHT,TREE_DEPTH));

    // TODO: Add star node (lighting)
    tree.attach_nodes(&star,NULL);
    build_lighting_node(star, octahedron, Wood, false, translate(TREE_X, 2.5f,TREE_Z)*scale(STAR_WIDTH,STAR_HEIGHT,STAR_DEPTH));
    // TODO: Add skirt node (texture)
    build_texture_node(skirt, circle, Skirt, translate(TREE_X,0.1f,TREE_Z)* scale(SKIRT_WIDTH,SKIRT_HEIGHT,SKIRT_DEPTH));
    skirt.attach_nodes(NULL,NULL);



}



void build_geometry( )
{
    // Generate vertex arrays and buffers
    glGenVertexArrays(NumVAOs, VAOs);

    // Load models
    load_object(Cube);
    load_object(Chair);
    load_object(Table);
    load_object(Sphere);
    load_object(Teapot);
    load_object(Mug);
    load_object(foxstatue);
    load_object(cylinder);
    load_object(cone);
    load_object(octahedron);
    load_object(circle);
    load_object(jet);
    load_object(Mirror);


    build_frame(Frame);
    // Generate color buffers
    glGenBuffers(NumColorBuffers, ColorBuffers);

    // Build color buffers
    // Define cube vertex colors (red)
    build_solid_color_buffer(numVertices[Cube], vec4(1.0f, 0.0f, 0.0f, 1.0f), RedCube);
    build_solid_color_buffer(numVertices[Cube], vec4(0.0f, 0.0f, 1.0f, 1.0f), BlueCube);

    build_solid_color_buffer(numVertices[Cube], vec4(0.0f, 0.5f, 0.5f, 1.0f), CyanCube);
    build_solid_color_buffer(numVertices[Cube], vec4(0.0f, 1.0f, 0.0f, 1.0f), GreenCube);
    build_solid_color_buffer(numVertices[Chair], vec4(1.0f, 1.0f, 0.0f, 1.0f), YellowChair);
    build_solid_color_buffer(numVertices[Table], vec4(1.0f, 0.0f, 1.0f, 1.0f), PurpleTable);
    build_solid_color_buffer(numVertices[Sphere], vec4(1.0f, 0.5f, 0.0f, 1.0f), SphereOrange);
    //build_solid_color_buffer(numVertices[jet],vec4(0.2f,0.2f,0.2f,1.0f),JetBlack);
    build_solid_color_buffer(numVertices[jet],vec4(1.0f, 0.0f, 0.0f, 1.0f),JetBlack);

}

void build_window_cube(GLuint obj) {
    vector<vec4> vertices;
    vector<vec3> normals;
    vector<vec2> uvCoords;

    // Define 3D vertices for cube
    vertices = {
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),    // front
            vec4(0.5f,  0.5f,  0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  0.5f, 1.0f),
            vec4(-0.5f, -0.5f, 0.5f, 1.0f),
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f),   // back
            vec4(-0.5f, 0.5f,  -0.5f, 1.0f),
            vec4(0.5f,  0.5f,  -0.5f, 1.0f),
            vec4(0.5f,  0.5f,  -0.5f, 1.0f),
            vec4(0.5f,  -0.5f, -0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f),
            vec4(0.5f, -0.5f, -0.5f, 1.0f),   // left
            vec4(0.5f, 0.5f, -0.5f, 1.0f),
            vec4(0.5f, 0.5f,  0.5f, 1.0f),
            vec4(0.5f, 0.5f,  0.5f, 1.0f),
            vec4(0.5f, -0.5f,  0.5f, 1.0f),
            vec4(0.5f, -0.5f, -0.5f, 1.0f),
            vec4(-0.5f,  -0.5f, -0.5f, 1.0f),   // right
            vec4(-0.5f,  -0.5f,  0.5f, 1.0f),
            vec4(-0.5f,  0.5f,  0.5f, 1.0f),
            vec4(-0.5f,  0.5f,  0.5f, 1.0f),
            vec4(-0.5f,  0.5f, -0.5f, 1.0f),
            vec4(-0.5f,  -0.5f, -0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  -0.5f, 1.0f),   // top
            vec4(-0.5f, 0.5f,  0.5f, 1.0f),
            vec4(0.5f,  0.5f,  0.5f, 1.0f),
            vec4(0.5f,  0.5f,  0.5f, 1.0f),
            vec4(0.5f,  0.5f,  -0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  -0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f),   // bottom
            vec4(0.5f,  -0.5f, -0.5f, 1.0f),
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),
            vec4(-0.5f, -0.5f, 0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f)
    };

    normals = {
            vec3(0.0f,  0.0f, 1.0f),    // Front
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(1.0f,  0.0f, -1.0f),   // Back
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(1.0f,  0.0f, 0.0f),    // Left
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),    // Right
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),     // Top
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),     // Bottom
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f)
    };

    // TODO: Define texture coordinates for fox
    uvCoords = {
            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),
            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),




    };


    // Set number of vertices
    numVertices[obj] = vertices.size();

    // Create and load object buffers
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);
    glBindVertexArray(VAOs[obj]);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*texCoords*numVertices[obj], uvCoords.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}




void build_fox_cube(GLuint obj) {
    vector<vec4> vertices;
    vector<vec3> normals;
    vector<vec2> uvCoords;

    // Define 3D vertices for cube
    vertices = {
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),    // front
            vec4(0.5f,  0.5f,  0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  0.5f, 1.0f),
            vec4(-0.5f, -0.5f, 0.5f, 1.0f),
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f),   // back
            vec4(-0.5f, 0.5f,  -0.5f, 1.0f),
            vec4(0.5f,  0.5f,  -0.5f, 1.0f),
            vec4(0.5f,  0.5f,  -0.5f, 1.0f),
            vec4(0.5f,  -0.5f, -0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f),
            vec4(0.5f, -0.5f, -0.5f, 1.0f),   // left
            vec4(0.5f, 0.5f, -0.5f, 1.0f),
            vec4(0.5f, 0.5f,  0.5f, 1.0f),
            vec4(0.5f, 0.5f,  0.5f, 1.0f),
            vec4(0.5f, -0.5f,  0.5f, 1.0f),
            vec4(0.5f, -0.5f, -0.5f, 1.0f),
            vec4(-0.5f,  -0.5f, -0.5f, 1.0f),   // right
            vec4(-0.5f,  -0.5f,  0.5f, 1.0f),
            vec4(-0.5f,  0.5f,  0.5f, 1.0f),
            vec4(-0.5f,  0.5f,  0.5f, 1.0f),
            vec4(-0.5f,  0.5f, -0.5f, 1.0f),
            vec4(-0.5f,  -0.5f, -0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  -0.5f, 1.0f),   // top
            vec4(-0.5f, 0.5f,  0.5f, 1.0f),
            vec4(0.5f,  0.5f,  0.5f, 1.0f),
            vec4(0.5f,  0.5f,  0.5f, 1.0f),
            vec4(0.5f,  0.5f,  -0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  -0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f),   // bottom
            vec4(0.5f,  -0.5f, -0.5f, 1.0f),
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),
            vec4(-0.5f, -0.5f, 0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f)
    };

    normals = {
            vec3(0.0f,  0.0f, 1.0f),    // Front
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(1.0f,  0.0f, -1.0f),   // Back
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(1.0f,  0.0f, 0.0f),    // Left
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),    // Right
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),     // Top
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),     // Bottom
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f)
    };

    // TODO: Define texture coordinates for fox
    uvCoords = {
            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),
            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),




    };


    // Set number of vertices
    numVertices[obj] = vertices.size();

    // Create and load object buffers
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);
    glBindVertexArray(VAOs[obj]);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*texCoords*numVertices[obj], uvCoords.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void build_lighton_cube(GLuint obj) {
    vector<vec4> vertices;
    vector<vec3> normals;
    vector<vec2> uvCoords;

    // Define 3D vertices for cube
    vertices = {
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),    // front
            vec4(0.5f,  0.5f,  0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  0.5f, 1.0f),
            vec4(-0.5f, -0.5f, 0.5f, 1.0f),
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f),   // back
            vec4(-0.5f, 0.5f,  -0.5f, 1.0f),
            vec4(0.5f,  0.5f,  -0.5f, 1.0f),
            vec4(0.5f,  0.5f,  -0.5f, 1.0f),
            vec4(0.5f,  -0.5f, -0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f),
            vec4(0.5f, -0.5f, -0.5f, 1.0f),   // left
            vec4(0.5f, 0.5f, -0.5f, 1.0f),
            vec4(0.5f, 0.5f,  0.5f, 1.0f),
            vec4(0.5f, 0.5f,  0.5f, 1.0f),
            vec4(0.5f, -0.5f,  0.5f, 1.0f),
            vec4(0.5f, -0.5f, -0.5f, 1.0f),
            vec4(-0.5f,  -0.5f, -0.5f, 1.0f),   // right
            vec4(-0.5f,  -0.5f,  0.5f, 1.0f),
            vec4(-0.5f,  0.5f,  0.5f, 1.0f),
            vec4(-0.5f,  0.5f,  0.5f, 1.0f),
            vec4(-0.5f,  0.5f, -0.5f, 1.0f),
            vec4(-0.5f,  -0.5f, -0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  -0.5f, 1.0f),   // top
            vec4(-0.5f, 0.5f,  0.5f, 1.0f),
            vec4(0.5f,  0.5f,  0.5f, 1.0f),
            vec4(0.5f,  0.5f,  0.5f, 1.0f),
            vec4(0.5f,  0.5f,  -0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  -0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f),   // bottom
            vec4(0.5f,  -0.5f, -0.5f, 1.0f),
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),
            vec4(-0.5f, -0.5f, 0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f)
    };

    normals = {
            vec3(0.0f,  0.0f, 1.0f),    // Front
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(1.0f,  0.0f, -1.0f),   // Back
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(1.0f,  0.0f, 0.0f),    // Left
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),    // Right
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),     // Top
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),     // Bottom
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f)
    };

    // TODO: Define texture coordinates for fox
    uvCoords = {
            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),
            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),




    };


    // Set number of vertices
    numVertices[obj] = vertices.size();

    // Create and load object buffers
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);
    glBindVertexArray(VAOs[obj]);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*texCoords*numVertices[obj], uvCoords.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void build_lightoff_cube(GLuint obj) {
    vector<vec4> vertices;
    vector<vec3> normals;
    vector<vec2> uvCoords;

    // Define 3D vertices for cube
    vertices = {
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),    // front
            vec4(0.5f,  0.5f,  0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  0.5f, 1.0f),
            vec4(-0.5f, -0.5f, 0.5f, 1.0f),
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f),   // back
            vec4(-0.5f, 0.5f,  -0.5f, 1.0f),
            vec4(0.5f,  0.5f,  -0.5f, 1.0f),
            vec4(0.5f,  0.5f,  -0.5f, 1.0f),
            vec4(0.5f,  -0.5f, -0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f),
            vec4(0.5f, -0.5f, -0.5f, 1.0f),   // left
            vec4(0.5f, 0.5f, -0.5f, 1.0f),
            vec4(0.5f, 0.5f,  0.5f, 1.0f),
            vec4(0.5f, 0.5f,  0.5f, 1.0f),
            vec4(0.5f, -0.5f,  0.5f, 1.0f),
            vec4(0.5f, -0.5f, -0.5f, 1.0f),
            vec4(-0.5f,  -0.5f, -0.5f, 1.0f),   // right
            vec4(-0.5f,  -0.5f,  0.5f, 1.0f),
            vec4(-0.5f,  0.5f,  0.5f, 1.0f),
            vec4(-0.5f,  0.5f,  0.5f, 1.0f),
            vec4(-0.5f,  0.5f, -0.5f, 1.0f),
            vec4(-0.5f,  -0.5f, -0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  -0.5f, 1.0f),   // top
            vec4(-0.5f, 0.5f,  0.5f, 1.0f),
            vec4(0.5f,  0.5f,  0.5f, 1.0f),
            vec4(0.5f,  0.5f,  0.5f, 1.0f),
            vec4(0.5f,  0.5f,  -0.5f, 1.0f),
            vec4(-0.5f, 0.5f,  -0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f),   // bottom
            vec4(0.5f,  -0.5f, -0.5f, 1.0f),
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),
            vec4(0.5f,  -0.5f, 0.5f, 1.0f),
            vec4(-0.5f, -0.5f, 0.5f, 1.0f),
            vec4(-0.5f, -0.5f, -0.5f, 1.0f)
    };

    normals = {
            vec3(0.0f,  0.0f, 1.0f),    // Front
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(1.0f,  0.0f, -1.0f),   // Back
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(1.0f,  0.0f, 0.0f),    // Left
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),    // Right
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(-1.0f,  0.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),     // Top
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),     // Bottom
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f),
            vec3(0.0f,  -1.0f, 0.0f)
    };

    // TODO: Define texture coordinates for fox
    uvCoords = {
            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),
            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),

            vec2(0.0f,0.0f),
            vec2(1.0f,0.0f),
            vec2(1.0f,1.0f),
            vec2(1.0f,1.0f),
            vec2(0.0f,1.0f),
            vec2(0.0f,0.0f),




    };


    // Set number of vertices
    numVertices[obj] = vertices.size();

    // Create and load object buffers
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);
    glBindVertexArray(VAOs[obj]);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normCoords*numVertices[obj], normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][TexBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*texCoords*numVertices[obj], uvCoords.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void build_materials( ) {
    // Add materials to Materials vector
    MaterialProperties CyanPlastic = {
            vec4(0.0f, 0.3f, 0.3f, 1.0f), //ambient
            vec4(0.0f, 0.6f, 0.6f, 1.0f), //diffuse
            vec4(0.0f, 0.8f, 0.8f, 1.0f), //specular
            32.0f, //shininess
            {0.0f, 0.0f, 0.0f}  //pad
    };

    MaterialProperties yellowAcrylic = {
            vec4(0.5f, 0.5f, 0.00f, 0.8f), //ambient
            vec4(0.70f, 0.70f, 0.0f, 0.8f), //diffuse
            vec4(0.85f, 0.85f, 0.0f, 0.8f), //specular
            14.8f, //shininess
            {0.0f, 0.0f, 0.0f}  //pad

    };

    MaterialProperties greenAcrylic = {
            vec4(0.0f, 0.5f, 0.00f, 0.6f), //ambient
            vec4(0.0f, 0.70f, 0.0f, 0.6f), //diffuse
            vec4(0.0f, 0.85f, 0.0f, 0.6f), //specular
            14.8f, //shininess
            {0.0f, 0.0f, 0.0f}  //pad


    };

    MaterialProperties redAcrylic = {
            vec4(0.5f, 0.0f, 0.00f, 0.6f), //ambient
            vec4(0.7f, 0.0f, 0.0f, 0.6f), //diffuse
            vec4(0.85f, 0.0f, 0.0f, 0.6f), //specular
            14.8f, //shininess
            {0.0f, 0.0f, 0.0f}  //pad


    };

    MaterialProperties cyanAcrylic = {
            vec4(0.0f, 0.3f, 0.50f, 1.0f), //ambient
            vec4(0.0f, 0.45f, 0.750f, 1.0f), //diffuse
            vec4(0.0f, 0.650f, 0.850f, 1.0f), //specular
            0.8f, //shininess
            {0.0f, 0.0f, 0.0f}  //pad


    };

    MaterialProperties wood = {
            vec4(0.5f, 0.35f, 0.05f,1.0f), //ambient
            vec4(0.5f, 0.35f, 0.05f,1.0f), //diffuse
            vec4(0.5f, 0.35f, 0.05f,1.0f), //specular
            6.8f, //shininess
            {0.0f, 0.0f, 0.0f}  //pad
    };

    MaterialProperties whiteBlind = {
            vec4(0.5f, 0.5f, 0.5f, 1.0f), //ambient
            vec4(0.7f, 0.7f, 0.7f, 1.0f), //diffuse
            vec4(1.0f, 1.0f, 1.0f, 1.0f), //specular
            14.8f, //shininess
            {0.0f, 0.0f, 0.0f}  //pad

    };

    MaterialProperties YellowStar = {
            vec4(0.7f, 0.7f, 0.0f, 1.0f), //ambient
            vec4(0.95f, 0.95f, 0.0f, 1.0f), //diffuse
            vec4(1.0f, 1.0f, 0.0f, 1.0f), //specular
            90.8f, //shininess
            {0.0f, 0.0f, 0.0f}  //pad

    };
    MaterialProperties Black = {
            vec4(0.3f, 0.3f, 0.3f, 1.0f), //ambient
            vec4(0.5f, 0.5f, 0.5f, 1.0f), //diffuse
            vec4(1.0f, 1.0f, 1.0f, 1.0f), //specular
            40.8f, //shininess
            {0.0f, 0.0f, 0.0f}  //pad

    };


    Materials.push_back(CyanPlastic);
    Materials.push_back(yellowAcrylic);
    Materials.push_back(greenAcrylic);
    Materials.push_back(redAcrylic);
    Materials.push_back(cyanAcrylic);
    Materials.push_back(wood);
    Materials.push_back(whiteBlind);
    Materials.push_back(YellowStar);
    Materials.push_back(Black);


    glGenBuffers(NumMaterialBuffers, MaterialBuffers);
    glBindBuffer(GL_UNIFORM_BUFFER, MaterialBuffers[MaterialBuffer]);
    glBufferData(GL_UNIFORM_BUFFER, Materials.size()*sizeof(MaterialProperties), Materials.data(), GL_STATIC_DRAW);
}


void build_lights( ) {
    // Add lights to Lights vector

    LightProperties whiteSpotLight = {
            DIRECTIONAL, //type
            {0.0f, 0.0f, 0.0f}, //pad
            vec4(0.0f, 0.0f, 0.0f, 1.0f), //ambient
            vec4(.70f, 0.7f, 0.7f, 1.0f), //diffuse
            vec4(.70f, 0.7f, 0.7f, 1.0f), //specular
            vec4(0.0f,4.0f,0.0f, 1.0f),  //position
            vec4(0.0f, -1.0f, 0.0f, 0.0f), //direction
            30.0f,   //cutoff
            30.0f,  //exponent
            {0.0f, 0.0f}  //pad2
    };

    LightProperties yellowSpotLight = {
            SPOT, //type
            {0.0f, 0.0f, 0.0f}, //pad
            vec4(0.0f, 0.0f, 0.0f, 1.0f), //ambient
            vec4(1.0f, 1.0f, 0.0f, 1.0f), //diffuse
            vec4(1.0f, 1.0f, 1.0f, 1.0f), //specular
            vec4(0.0f, 5.0f, 0.0f, 1.0f),  //position
            vec4(0.0f, -1.0f, 0.0f, 0.0f), //direction
            50.0f,   //cutoff
            50.0f,  //exponent
            {0.0f, 0.0f}  //pad2
    };

    LightProperties yellowSpotLight2 = {
            SPOT, //type
            {0.0f, 0.0f, 0.0f}, //pad
            vec4(0.0f, 0.0f, 0.0f, 1.0f), //ambient
            vec4(1.0f, 1.0f, 0.0f, 1.0f), //diffuse
            vec4(1.0f, 1.0f, 1.0f, 1.0f), //specular
            vec4(0.0f, 0.0f, 0.0f, 1.0f),  //position
            vec4(0.0f, 1.0f, 0.0f, 0.0f), //direction
            50.0f,   //cutoff
            50.0f,  //exponent
            {0.0f, 0.0f}  //pad2
    };


    LightProperties whiteSpotLight2 = {
            POINT, //type
            {0.0f, 0.0f, 0.0f}, //pad
            vec4(0.0f, 0.0f, 0.0f, 1.0f), //ambient
            vec4(.70f, 0.7f, 0.7f, 1.0f), //diffuse
            vec4(.70f, 0.7f, 0.7f, 1.0f), //specular
            vec4(0.0f,0.0f,0.0f, 1.0f),  //position
            vec4(0.0f, 1.0f, 0.0f, 0.0f), //direction
            30.0f,   //cutoff
            30.0f,  //exponent
            {0.0f, 0.0f}  //pad2
    };

    // Add lights to Lights vector

    Lights.push_back(whiteSpotLight);
    Lights.push_back(yellowSpotLight);
    Lights.push_back(yellowSpotLight2);
    Lights.push_back(whiteSpotLight2);
    // Set numLights
    numLights = Lights.size();

    // Turn all lights on
    for (int i = 0; i < numLights; i++) {
        lightOn[i] = 1;
    }

    // Create uniform buffer for lights
    glGenBuffers(NumLightBuffers, LightBuffers);
    glBindBuffer(GL_UNIFORM_BUFFER, LightBuffers[LightBuffer]);
    glBufferData(GL_UNIFORM_BUFFER, Lights.size()*sizeof(LightProperties), Lights.data(), GL_STATIC_DRAW);
}


void draw_frame(GLuint obj){
    // Draw frame using lines at mirror location
    glUseProgram(lighting_program);
    // Pass projection and camera matrices to shader
    glUniformMatrix4fv(lighting_proj_mat_loc, 1, GL_FALSE, proj_matrix);
    glUniformMatrix4fv(lighting_camera_mat_loc, 1, GL_FALSE, camera_matrix);

    // Bind lights
    glUniformBlockBinding(lighting_program, lighting_lights_block_idx, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, LightBuffers[LightBuffer], 0, Lights.size()*sizeof(LightProperties));
    // Bind materials
    glUniformBlockBinding(lighting_program, lighting_materials_block_idx, 1);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, MaterialBuffers[MaterialBuffer], 0, Materials.size()*sizeof(MaterialProperties));
    // Set camera position
    glUniform3fv(lighting_eye_loc, 1, eye);
    // Set num lights and lightOn
    glUniform1i(lighting_num_lights_loc, numLights);
    glUniform1iv(lighting_light_on_loc, numLights, lightOn);

    // Set frame transformation matrix
    mat4 trans_matrix = translate(mirror_eye);
    mat4 rot_matrix = rotate(-90.0f, vec3(1.0f, 0.0f, 0.0f));
    mat4 scale_matrix = scale(1.0f, 2.0f, 1.0f);
    model_matrix = trans_matrix * rot_matrix * scale_matrix;
    // Compute normal matrix from model matrix
    normal_matrix = model_matrix.inverse().transpose();
    // Pass model matrix and normal matrix to shader
    glUniformMatrix4fv(lighting_model_mat_loc, 1, GL_FALSE, model_matrix);
    glUniformMatrix4fv(lighting_norm_mat_loc, 1, GL_FALSE, normal_matrix);
    glUniform1i(lighting_material_loc, CyanPlastic);

    // Draw object using line loop
    glBindVertexArray(VAOs[obj]);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glVertexAttribPointer(lighting_vPos, posCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(lighting_vPos);
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][NormBuffer]);
    glVertexAttribPointer(lighting_vNorm, normCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(lighting_vNorm);
    glDrawArrays(GL_LINE_LOOP, 0, numVertices[obj]);

}





void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // ESC to quit
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        blindRotate = true;
    }
    if (key == GLFW_KEY_J ) {

        pouring = true;
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS){
        if(tree_spin == true)
        {
            tree_spin = false;
        }
        else if(tree_spin == false)
        {
            tree_spin = true;
        }
    }

    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        if(lightOn[0] == 1)
        {
            lightOn[0] = 0;
            lightOn[3] = 0;
        }
        else if(lightOn[0] == 0)
        {
            lightOn[0] = 1;
            lightOn[3] = 1;
        }
    }


    if(key== GLFW_KEY_E && action == GLFW_PRESS)
    {
        if(jet_fly == false)
        {
            jet_fly = true;
        }
        else if(jet_fly == true)
        {
            jet_fly = false;
        }
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        if(lightswitchon == 1)
        {
            lightswitchon = 0;
            lightOn[1] = 0;
            lightOn[2] = 0;
            fan_spin = false;
        }
        else if(lightswitchon == 0)
        {
            lightswitchon = 1;
            lightOn[1] = 1;
            lightOn[2] = 1;
            fan_spin = true;
        }
    }

    if(key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        if(jet_radius>=1.0f)
        {
            jet_radius = jet_radius - 0.10f;
        }
    }
    if(key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        if(jet_radius<=4.0f)
        {
            jet_radius = jet_radius + 0.10f;
        }
    }
    if(key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        if(jet_height<=3.0f)
        {
            jet_height = jet_height + 0.10f;
        }
    }
    if(key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        if(jet_height>=0.50f)
        {
            jet_height = jet_height - 0.10f;
        }
    }

// CAMERA STUFF
    // Adjust azimuth
    if (key == GLFW_KEY_A) {
        azimuth -= daz;
        if (azimuth > 360.0) {
            azimuth += 360.0;
        }
    } else if (key == GLFW_KEY_D) {
        azimuth += daz;
        if (azimuth < 0.0)
        {
            azimuth -= 360.0;
        }
    }

    // Adjust elevation angle
    if (key == GLFW_KEY_W)
    {
        //elevation += del;
        //if (elevation > 179.0)
        //{
        //    elevation = 179.0;
        //}
        GLfloat dirx, dirz;

        dirx = center[0] - eye[0];
        dirz = center[2] - eye[2];

        eye[0] = eye[0] + dirx*.1;
        eye[2] = eye[2] + dirz*.1;



    }
    else if (key == GLFW_KEY_S)
    {

        //elevation -= del;
        //if (elevation > 179.0)
        //{
        //    elevation = 179.0;
        //}
        GLfloat dirx, dirz;

        dirx = center[0] - eye[0];
        dirz = center[2] - eye[2];

        eye[0] = eye[0] - dirx*.1;
        eye[2] = eye[2] - dirz*.1;

    }
    // Adjust radius (zoom)
    if (key == GLFW_KEY_X)
    {
        radius += dr;
    }
    else if (key == GLFW_KEY_Z)
    {
        radius -= dr;
        if (radius < min_radius)
        {
            radius = min_radius;
        }
    }
    if(key == GLFW_KEY_B )
    {
        blindRotate = true;
    }

    GLfloat dx, dy, dz;
    dx = eye[0] + cos(azimuth*DEG2RAD);
    dy = eye[1];
    dz = eye[2] + sin(azimuth*DEG2RAD);

    center = vec3(dx,dy,dz);



}

void mouse_callback(GLFWwindow *window, int button, int action, int mods){

}


unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad(GLuint shader, GLuint tex) {
    // reset viewport
    glViewport(0, 0, ww, hh);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render Depth map to quad for visual debugging
    // ---------------------------------------------
    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureIDs[tex]);
    if (quadVAO == 0) {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

#include "utilfuncs.cpp"