 #version 330 core
 layout (location = 0) in vec3 aPos;
 layout (location = 1) in vec3 aNor;
 layout (location = 2) in vec2 aTex;
 
 out vec2 ourTex;
 out vec3 ourNorm;
 out vec3 FragPos;
 
 uniform mat4 model;
 uniform mat4 trmodel;
 uniform mat4 vp;
 uniform mat4 mvp;
 
 uniform float x_size;
 uniform float y_size;
 uniform float z_size;
 uniform float x_delta;
 uniform float y_delta;
 
 uniform sampler2D height_texture;
 
 void main()
 {
     ourTex = aTex;

     vec3 pos = aPos;

     if(pos.z < -10.0f) {
        vec2 tex = vec2(pos.x/x_size + 0.5f,pos.y/y_size + 0.5f );
        pos.z = texture(height_texture, tex).r;
     }
     vec3 Normal = aNor;
     if(aNor.z < -10.0f) {
        vec2 tex = vec2(pos.x/x_size + 0.5f,pos.y/y_size + 0.5f );
        float delta_x_ = x_delta/x_size;
        float delta_y_ = y_delta/y_size;

        vec3 left = vec3(pos.x - x_delta, pos.y, texture(height_texture, tex - vec2(delta_x_,0)).r);
        vec3 right = vec3(pos.x + x_delta, pos.y, texture(height_texture, tex + vec2(delta_x_,0)).r);

        vec3 up = vec3(pos.x, pos.y + y_delta, texture(height_texture, tex + vec2(0,delta_y_)).r);
        vec3 down = vec3(pos.x, pos.y - y_delta, texture(height_texture, tex - vec2(0,delta_y_)).r);

        vec3 lr = right - left;
        vec3 du = up - down;

        Normal = normalize(cross(lr,du));
     }
     
     vec4 color = vec4(1.0f,0.0f,0.0f,1.0f);

     ourNorm = mat3(trmodel) * Normal;
     FragPos = vec3(model * vec4(pos, 1.0));;

     gl_Position =  vp * vec4(FragPos, 1.0);
 }
