#include "entity.h"

#include "includes.h"
#include "texture.h"
#include "mesh.h"
#include "shader.h"
#include <iostream>


Entity::Entity(std::string name)
{
	Matrix44 model = this->model;
	shader = Shader::Load("data/shaders/basic.vs", "data/shaders/texture.fs");
}

Entity::~Entity()
{

}

void Entity::render() 
{

	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	Camera::current->enable();

	glDisable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	if (shader)
	{
		//enable shader
	    shader->enable();

		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", Camera::current->viewprojection_matrix);
		shader->setUniform("u_texture", texture);
		shader->setUniform("u_model", model);

		//current_shader->setUniform("u_model", m);
		Mesh* mesh = new Mesh();
		mesh = Mesh::Load(mesh_name.c_str());

		mesh->render(GL_TRIANGLES, shader);

		//disable shader
		shader->disable();
	}
	
	//if this was an EntityMesh...
	//Matrix44 global_matrix = getGlobalMatrix();
	//renderMesh( global_matrix );
	for (int i = 0; i < children.size(); i++)
		children[i]->render(); //repeat for every child
}

void Entity::update(float elapsed_time)
{

}

void Entity::getPosition()
{

}

void Entity::addChild(Entity* ent)
{

}

void Entity::removeChild(Entity* ent)
{

}

//get the global transformation of this object (not the relative to the parent)
//this function uses recursivity to crawl the tree upwards
Matrix44 Entity::getGlobalMatrix()
{
	if (parent) //if I have a parent, ask his global and concatenate
		return model * parent->getGlobalMatrix();
	return model; //otherwise just return my model as global
}

Airplane::Airplane(std::string name) : Entity(name)
{
	mesh_name = "data/assets/bomber/bomber_axis.ASE";
	texture_name = "data/assets/bomber/bomber_axis.tga";
	texture = new Texture();
	texture->load(texture_name.c_str());
}

Airplane::~Airplane()
{
}

void Airplane::applyLookAt(Camera * camera)
{
}

void Airplane::update(float elapsed_time)
{
}
