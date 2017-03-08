#include "Cube.hpp"
#include "Scene.hpp"
#include <glm/gtc/type_ptr.hpp>

const float Cube::vertices[108] = {-1, -1, -1,   1, -1, -1,   1, 1, -1,     // Face 1
				   -1, -1, -1,   -1, 1, -1,   1, 1, -1,     // Face 1

				   1, -1, 1,   1, -1, -1,   1, 1, -1,       // Face 2
				   1, -1, 1,   1, 1, 1,   1, 1, -1,         // Face 2

				   -1, -1, 1,   1, -1, 1,   1, -1, -1,      // Face 3
				   -1, -1, 1,   -1, -1, -1,   1, -1, -1,    // Face 3

				   -1, -1, 1,   1, -1, 1,   1, 1, 1,        // Face 4
				   -1, -1, 1,   -1, 1, 1,   1, 1, 1,        // Face 4

				   -1, -1, -1,   -1, -1, 1,   -1, 1, 1,     // Face 5
				   -1, -1, -1,   -1, 1, -1,   -1, 1, 1,     // Face 5

				   -1, 1, 1,   1, 1, 1,   1, 1, -1,         // Face 6
				   -1, 1, 1,   -1, 1, -1,   1, 1, -1};

const float Cube::normals[108] =  {0.0, 0.0, -1.0,   0.0, 0.0, -1.0,   0.0, 0.0, -1.0,           // Face 1
				   0.0, 0.0, -1.0,   0.0, 0.0, -1.0,   0.0, 0.0, -1.0,           // Face 1

				   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 2
				   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 2

				   0.0, -1.0, 0.0,   0.0, -1.0, 0.0,   0.0, -1.0, 0.0,           // Face 3
				   0.0, -1.0, 0.0,   0.0, -1.0, 0.0,   0.0, -1.0, 0.0,           // Face 3

				   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 4
				   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 4

				   -1.0, 0.0, 0.0,   0.0, 0.0, -1.0,   0.0, 0.0, -1.0,           // Face 5
				   -1.0, 0.0, 0.0,   0.0, 0.0, -1.0,   0.0, 0.0, -1.0,           // Face 5

				   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 6
				   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0};

const  float Cube::coordTexture[72] =  {0, 0,   1, 0,   1, 1,
					0, 0,   0, 1,   1, 1,

					0, 0,   1, 0,   1, 1,
					0, 0,   0, 1,   1, 1,

					0, 0,   1, 0,   1, 1,
					0, 0,   0, 1,   1, 1,

					0, 0,   1, 0,   1, 1,
					0, 0,   0, 1,   1, 1,

					0, 0,   1, 0,   1, 1,
					0, 0,   0, 1,   1, 1,

					0, 0,   1, 0,   1, 1,
					0, 0,   0, 1,   1, 1};

Cube::Cube(float s, Shader* shader, Texture* texture): Object(shader, texture) {
    size = s;
    //m_model_view = translate(m_model_view, glm::vec3(0.0, 5.0, 0.0));
    m_model_view = scale(m_model_view, glm::vec3(s, s, s));
    
  }

  void Cube::animate() {
    m_model_view = rotate(m_model_view, (float)0.02, glm::vec3(0, 1, 0));
  // m_model_view = translate(m_model_view, glm::vec3(0.0, 5.0, 0.0));
  }
  
  void Cube::draw(glm::mat4 m, Shader *s) {
    float couleurs[108] = {1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 1
		1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 1

		0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 2
		0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 2

		0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 3
		0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 3

		1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 4
		1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 4

		0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 5
		0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 5

		0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 6
		0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0};
    enableShader();
    setMVP(m, s);
    // glm::mat4 vp;
    // Scene::SCENE->getProjView(vp);
    // glm::mat4 mvp = vp * m_model_view;
    //   glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgramID(), "mvp"), 1, GL_FALSE, value_ptr(mvp));
      
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, couleurs);
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, normals);
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, coordTexture);
  glEnableVertexAttribArray(3);

  glDrawArrays(GL_TRIANGLES, 0, 36);

  glDisableVertexAttribArray(3);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);

  disableShader();
  }
