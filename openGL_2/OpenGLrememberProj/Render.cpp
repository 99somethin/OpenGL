#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"
#include <vector>
#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


void create_edges(double x)
{
	


	glBegin(GL_TRIANGLES);


	double A[] = { 0,0,x };
	double E[] = { -2,5,x };
	double F[] = { 2,7,x };
	double G[] = { 10,1,x };
	double D[] = { 1,-2,x };
	double K[] = { 9,-4,x };

	glColor3d(0.3, 0.2, 0.5);



	glNormal3f(0.0, 0.0, x > 0 ? 1.0 : -1.0);

	glTexCoord2d(0.45  , 0.5 );
	glVertex3dv(A);
	glTexCoord2d(0.35 , 0.75 );
	glVertex3dv(E);
	glTexCoord2d(0.55 , 0.85 );
	glVertex3dv(F);


	glNormal3f(0.0, 0.0, x > 0 ? 1.0 : -1.0); 

	glTexCoord2d(0.45 * 2, 0.5 * 2);
	glVertex3dv(A);
	glTexCoord2d(0.95 * 2, 0.55 * 2);
	glVertex3dv(G);
	glTexCoord2d(0.55 * 2, 0.85 * 2);
	glVertex3dv(F);


	glNormal3f(0.0, 0.0, x > 0 ? 1.0 : -1.0); 

	glTexCoord2d(0.5 * 2, 0.40 * 2);
	glVertex3dv(D);
	glTexCoord2d(0.95 * 2, 0.55 * 2);
	glVertex3dv(G);
	glTexCoord2d(0.45 * 2, 0.5 * 2);
	glVertex3dv(A);


	glNormal3f(0.0, 0.0, x > 0 ? 1.0 : -1.0);

	glTexCoord2d(0.5 * 2, 0.40 * 2);
	glVertex3dv(D);
	glTexCoord2d(0.95 * 2, 0.55 * 2);
	glVertex3dv(G);
	glTexCoord2d(0.9 * 2, 0.30 * 2);
	glVertex3dv(K);


	glEnd();
}




float normal[] = { 0,0,0 };

void vector_colculate(std::vector <float> mas, std::vector <float> mas2)
{
	normal[0] = (mas[1] * mas2[2] - mas[2] * mas2[1]);
	normal[1] = (mas[0] * mas2[2] - mas2[0] * mas[2]) * (-1);
	normal[2] = (mas[0] * mas2[1] - mas[1] * mas2[0]);
}


void create_side_edges()
{
	glBegin(GL_QUADS);

	glColor3d(0.3, 0.8, 0.5);


	std :: vector <float> vector_first = { -5,-3,0 };
	std :: vector <float> vector_second = { 0,0,1 };
	vector_colculate(vector_first, vector_second);
	glNormal3f(normal[0],normal[1],normal[2]);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 1);
	glVertex3d(-5, -3, 1);
	glVertex3d(-5, -3, 0);

	vector_first = { -2,-7,0 };
	vector_second = { 0,0,-1 };
	vector_colculate(vector_first, vector_second);
	glNormal3f(normal[0], normal[1], normal[2]);
	glVertex3d(-1, -9, 0);
	glVertex3d(-1, -9, 1);
	glVertex3d(1, -2, 1);
	glVertex3d(1, -2, 0);

	vector_first = { -8,2,0 };
	vector_second = { 0,0,-1 };
	vector_colculate(vector_first, vector_second);
	glNormal3f(normal[0], normal[1], normal[2]);
	glVertex3d(1, -2, 1);
	glVertex3d(1, -2, 0);
	glVertex3d(9, -4, 0);
	glVertex3d(9, -4, 1);

	vector_first = { -1,-5,0 };
	vector_second = { 0,0,-1 };
	vector_colculate(vector_first, vector_second);
	glNormal3f(normal[0], normal[1], normal[2]);
	glVertex3d(9, -4, 0);
	glVertex3d(9, -4, 1);
	glVertex3d(10, 1, 1);
	glVertex3d(10, 1, 0);

	vector_first = { -4,-2,0 };
	vector_second = { 0,0,1 };
	vector_colculate(vector_first, vector_second);
	glNormal3f(normal[0], normal[1], normal[2]);
	glVertex3d(2, 7, 0);
	glVertex3d(2, 7, 1);
	glVertex3d(-2, 5, 1);
	glVertex3d(-2, 5, 0);

	vector_first = { -2,5,0 };
	vector_second = { 0,0,-1 };
	vector_colculate(vector_first, vector_second);
	glNormal3f(normal[0], normal[1], normal[2]);
	glVertex3d(-2, 5, 1);
	glVertex3d(-2, 5, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 1);

	glEnd();
}



void create_circle_edge()
{
	
	const float radius = 5;
	const int segments = 50;

	glPushMatrix();
	glTranslated(6, 4, 0);
	glRotated(-37, 0, 0, 1);


	
	glNormal3f(0, 0, -1);
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.3, 0.2, 0.5);
	glVertex3d(0, 0, 0); 
	for (int i = 0; i <= segments; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(segments); 
		if (theta <= 3.4)
		{
			float x = radius * cosf(theta); 
			float y = radius * sinf(theta); 
			glVertex3d(x, y, 0); 
		}
	}
	glEnd();

	
	glNormal3f(0, 0, 1);
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.3, 0.2, 0.5);
	glTexCoord2f(0.5, 0.5); 
	glVertex3d(0, 0, 1); 
	for (int i = 0; i <= segments; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(segments); 
		if (theta <= 3.4)
		{
			float x = radius * cosf(theta); 
			float y = radius * sinf(theta); 
			glTexCoord2f((x + radius) / (2 * radius), (y + radius) / (2 * radius));
			glVertex3d(x, y, 1); 
		}
	}
	glEnd();

	glPopMatrix();
	
}



void create_circle_side()
{
	float radius = 5;
	int segments = 50;

	glPushMatrix();

	glTranslated(6, 4, 1);

	glRotated(-90, 1, 0, 0);

	glRotated(37, 0, 1, 0);

	glColor3d(0.9, 0.9, 0.5);

	glBegin(GL_TRIANGLE_STRIP);

	for (int i = 0; i <= segments; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(segments); 

		if (theta <= 3.4)
		{
			float x = radius * cosf(theta); 
			float z = radius * sinf(theta);

			
			glNormal3f(x, 0, z);
			glVertex3f(x, 1, z);

			
			glNormal3f(x, 0, z); 
			glVertex3f(x, 0, z);
		}
	}
	glEnd();

	glPopMatrix();
}


void internal()
{
	
	float radius = 8.06;
	
	float segments = 800;

	glPushMatrix();

	glTranslated(-9, -10, 0);

	glNormal3f(0, 0, -1);

	glBegin(GL_TRIANGLE_FAN);

	glColor3d(0.3, 0.2, 0.5);

	glVertex3d(10, 8, 0); 

	for (int i = 0; i <= segments / 5; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(segments); 


		float x = radius * cosf(theta); 
		float y = radius * sinf(theta); 

		if (x <= 8.003 && y <= 7.03f)
		{
			glVertex3d(x, y, 0); 
			
		}

	}

	glEnd();

	glPopMatrix();


	glBegin(GL_TRIANGLES);

	glNormal3f(0, 0, -1);

	glVertex3d(0, 0, 0);
	glVertex3d(1, -2, 0);
	glVertex3d(-5, -3, 0);

	glEnd();


	glPushMatrix();

	glTranslated(-9, -10, 0);

	glNormal3f(0, 0, 1);

	glBegin(GL_TRIANGLE_FAN);


	glColor3d(0.3, 0.2, 0.5);
	glTexCoord2f(0.5, 0.5); 
	glVertex3d(10, 8, 1); 


	for (int i = 0; i <= segments / 5; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(segments); 

		float x = radius * cosf(theta);
		float y = radius * sinf(theta); 

		
		float s = 0.5 * (1 + cosf(theta)); 
		float t =  0.5 * (1 + sinf(theta));

		if (x <= 8.003 && y <= 7.03f)
		{
			glTexCoord2f(s, t); 
			glVertex3d(x, y, 1); 
		}
	}
	glEnd();


	glPopMatrix();


	glBegin(GL_TRIANGLES);

	glNormal3f(0, 0, 1);

	glTexCoord2d(0.45 * 2, 0.5 * 2);
	glVertex3d(0, 0, 1);
	glTexCoord2d(0.5 * 2, 0.40 * 2);
	glVertex3d(1, -2, 1);
	glTexCoord2d(0.2 * 2, 0.35 * 2);
	glVertex3d(-5, -3, 1);

	glEnd();

}


void internal_circle()
{
	float radius = 8.06;
	int segments = 1000;

	glPushMatrix();

	glTranslated(-9, -10, 1);
	
	glRotated(-90, 1, 0, 0);

	glRotated(-7, 0, 1, 0);

	glBegin(GL_TRIANGLE_STRIP);

	glColor3d(0.9, 0.9, 0.5);

	for (int i = 0; i <= segments / 6; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(segments); 


		float x = radius * cosf(theta); 
		float z = radius * sinf(theta); 

		if (x >= 4.80)
		{
			
			glNormal3f(-x, 0, -z);
			glVertex3f(x, 1, z);
			
			glNormal3f(-x, 0, -z);
			glVertex3f(x, 0, z);
		}
	}
	glEnd();

	glPopMatrix();
}




void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);





	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������

	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

	glBindTexture(GL_TEXTURE_2D, texId);

	int high_edge = 1;
	int down_edge = 0;

	create_edges(down_edge);
	create_edges(high_edge);
	create_side_edges();

	create_circle_edge();
	create_circle_side();

	internal();
	internal_circle();



	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}