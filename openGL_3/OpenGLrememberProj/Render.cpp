#include "Render.h"
#include <vector>
#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>


double t = 0.0; // Параметр для движения по кривой Безье
bool forwardDirection = true;


inline double fBezie(double p1, double p2, double p3,double p4, double t)
{
	double oneMinusT = 1.0f - t;

	return oneMinusT * oneMinusT * oneMinusT * p1 + 3.0f * oneMinusT * oneMinusT * t * p2 + 3.0f * oneMinusT * t * t * p3 + t * t * t * p4;
}

inline double ProizvodBezie(double p1, double p2, double p3, double p4, double t)
{
	double oneMinusT = 1.0f - t;

	return 3.0f * oneMinusT * oneMinusT * (p2 - p1) + 6.0f * oneMinusT * t * (p3 - p2) + 3.0f * t * t * (p4 - p3);
}

inline double fErmit(double p1, double p4, double r1, double r4, double t)
{
	return p1 * (2 * t * t * t - 3 * t * t + 1) + p4 * (-2 * t * t * t + 3 * t * t) + r1 * (t * t * t - 2 * t * t + t) + r4 * (t * t * t - t * t);
}


void writeBizie(double P1[], double P2[], double P3[], double P4[])
{
	glLineWidth(3); //ширина линии

	glBegin(GL_LINE_STRIP);

	for (double t = 0; t <= 1.0001; t += 0.01)
	{
		double P[3];

		P[0] = fBezie(P1[0], P2[0], P3[0], P4[0], t);
		P[1] = fBezie(P1[1], P2[1], P3[1], P4[1], t);
		P[2] = fBezie(P1[2], P2[2], P3[2], P4[2], t);
		

		glVertex3dv(P); //Рисуем точку P
	}

	glEnd();
	glLineWidth(1); //возвращаем ширину линии = 1

	glLineWidth(3); //ширина линии

	glBegin(GL_LINE_STRIP);

	glVertex3dv(P1);

	glVertex3dv(P2);

	glVertex3dv(P3);

	glVertex3dv(P4);

	glEnd();
	glLineWidth(1);
}


void writeErmit(double P1[],double P4[], double R1[], double R4[])
{
	glLineWidth(3); //ширина линии

	glBegin(GL_LINE_STRIP);

	glVertex3dv(P1);

	glVertex3dv(R1);

	glEnd();
	glLineWidth(1);

	glLineWidth(3); //ширина линии

	glBegin(GL_LINE_STRIP);

	glVertex3dv(P4);

	glVertex3dv(R4);

	glEnd();
	glLineWidth(1);

	R1[0] = R1[0] - P1[0];
	R1[1] = R1[1] - P1[1];
	R1[2] = R1[2] - P1[2];

	R4[0] = R4[0] - P4[0];
	R4[1] = R4[1] - P4[1];
	R4[2] = R4[2] - P4[2];

	glLineWidth(3); //ширина линии

	glBegin(GL_LINE_STRIP);

	for (double t = 0; t <= 1.0001; t += 0.01)
	{
		double P[3];

		P[0] = fErmit(P1[0], P4[0], R1[0], R4[0], t);
		P[1] = fErmit(P1[1], P4[1], R1[1], R4[1], t);
		P[2] = fErmit(P1[2], P4[2], R1[2], R4[2], t);

		glVertex3dv(P); //Рисуем точку P
	}
	glEnd();
	glLineWidth(1); //возвращаем ширину линии = 1

	

}




void Render(double delta_time)
{   

	double P1[] = { 10,-15,0 };
	double P2[] = { 15,0,0 };
	double P3[] = { 0,15,10 };
	double P4[] = { -15,15,0 };

		writeBizie(P1, P2, P3, P4);


		double P5[] = { 5,5,0 };
		double P6[] = { 10,10,5 };
		double P7[] = { 20,17,3 };
		double P8[] = { 25,-26,3 };

		double bezierPoint[3];
		bezierPoint[0] = fBezie(P1[0], P2[0], P3[0], P4[0], t);
		bezierPoint[1] = fBezie(P1[1], P2[1], P3[1], P4[1], t);
		bezierPoint[2] = fBezie(P1[2], P2[2], P3[2], P4[2], t);


		double bezierProizdov[3];
		bezierProizdov[0] = ProizvodBezie(P1[0], P2[0], P3[0], P4[0], t);
		bezierProizdov[1] = ProizvodBezie(P1[1], P2[1], P3[1], P4[1], t);
		bezierProizdov[2] = ProizvodBezie(P1[2], P2[2], P3[2], P4[2], t);

		double tangentLength = sqrt(bezierProizdov[0] * bezierProizdov[0] +
			bezierProizdov[1] * bezierProizdov[1] +
			bezierProizdov[2] * bezierProizdov[2]);

		double tangentDirection[3];
		tangentDirection[0] = bezierProizdov[0] / tangentLength;
		tangentDirection[1] = bezierProizdov[1] / tangentLength;
		tangentDirection[2] = bezierProizdov[2] / tangentLength;


		glPushMatrix(); 

		
		glTranslatef(bezierPoint[0], bezierPoint[1], bezierPoint[2]); 
		

		float angleX = atan2(-tangentDirection[1], tangentDirection[2]) * 180.0f / 3.1415f;
		float angleY = atan2(tangentDirection[0], sqrt(tangentDirection[1] * tangentDirection[1] +
			tangentDirection[2] * tangentDirection[2])) * 180.0f / 3.1415f;

		// Поворот объекта
		glRotatef(angleX, 1.0f, 0.0f, 0.0f);
		glRotatef(angleY, 0.0f, 1.0f, 0.0f);

		glColor3f(1.0f, 0.0f, 0.0f); 

		// куб
		glBegin(GL_LINES); 
		glVertex3f(-0.5f, -0.5f, -0.5f); 
		glVertex3f(0.5f, -0.5f, -0.5f);

		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);

		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, -0.5f);

		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

		glVertex3f(-0.5f, -0.5f, 0.5f); 
		glVertex3f(0.5f, -0.5f, 0.5f);

		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);

		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);

		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);

		glVertex3f(-0.5f, -0.5f, -0.5f); 
		glVertex3f(-0.5f, -0.5f, 0.5f);

		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);

		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);

		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glEnd();

		glPopMatrix();

		
		if (forwardDirection)
		{
			t += 0.1 * delta_time;
			if (t > 1.0)
			{
				t = 1.0;
				forwardDirection = false; 
			}
		}
		else
		{
			t -= 0.1 * delta_time;
			if (t < 0.0)
			{
				t = 0.0;
				forwardDirection = true; 
			}
		}


		writeBizie(P5, P6, P7, P8);

		double P13[] = { -1,0,0 };
		double P14[] = { -1,0,5 };
		double P15[] = { -10,10,0 };
		double P16[] = { -11,11,-1 };

		writeErmit(P13, P15, P14, P16);


		double P9[] = { -1,-1,0 };
		double P10[] = { -1,-1,5 };
		double P11[] = { -10,-10,0 };
		double P12[] = { -11,-11,-1 };

		writeErmit(P9, P11, P10, P12);

}   

