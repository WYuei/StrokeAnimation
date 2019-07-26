#pragma once
#include<cmath>
#include<opencv.hpp>
using namespace std;
using namespace cv;

const double PI = acos(-1.0);
const double GaussEps = 0.003;
const int MATRIX_SIZE = 4;

struct Matrix
{
	double a[MATRIX_SIZE][MATRIX_SIZE];

	Matrix()
	{
		for (int i = 0; i < MATRIX_SIZE; i++)
			for (int j = 0; j < MATRIX_SIZE; j++)
				a[i][j] = 0;
	}

	const Matrix zero()
	{
		return Matrix();
	}

	const Matrix unit()
	{
		Matrix ret= Matrix();
		memset(ret.a, 0, sizeof(ret.a));
		for (int i = 0; i < MATRIX_SIZE; i++)
			ret.a[i][i] = 1;
		return ret;
	}


	Matrix operator *(const Matrix &m)const
	{
		Matrix ret= Matrix();
		for(int i=0;i<MATRIX_SIZE;i++)
			for (int k = 0; k < MATRIX_SIZE; k++)
			{
				if (a[i][k] == 0)
					continue;
				for (int j = 0; j < MATRIX_SIZE; j++)
					ret.a[i][j] += a[i][k] * m.a[k][j];
			}
		return ret;
	}
	Matrix operator +(const Matrix &m)const
	{
		Matrix ret = Matrix();
		for (int i = 0; i < MATRIX_SIZE; i++)
			for (int j = 0; j < MATRIX_SIZE; j++)
				ret.a[i][j] = a[i][j] + m.a[i][j];
		return ret;
	}

	Matrix operator -(const Matrix &m)const
	{
		Matrix ret = Matrix();
		for (int i = 0; i < MATRIX_SIZE; i++)
			for (int j = 0; j < MATRIX_SIZE; j++)
				ret.a[i][j] = a[i][j] - m.a[i][j];
		return ret;
	}

	Vec2f operator *(const Vec2f &v)const
	{
		Matrix ret = Matrix();
		ret.a[0][0] = v[0];
		ret.a[1][0] = v[1];
		ret.a[3][0] = 1;
		ret = (*this) * ret;
		return Vec2f(ret.a[0][0], ret.a[1][0]);
	}

	Vec3f operator *(const Vec3f &v)const
	{
		Matrix ret = Matrix();
		ret.a[0][0] = v[0];
		ret.a[1][0] = v[1];
		ret.a[2][0] = v[2];
		ret.a[3][0] = 1;
		ret = (*this) * ret;
		return Vec3f(ret.a[0][0], ret.a[1][0], ret.a[2][0]);
	}
};

Matrix MatrixFractionalPow(Matrix m, double p)
{
	m.a[0][3] *= p;
	m.a[1][3] *= p;
	m.a[2][3] *= p;
	return m;
}

Matrix MatrixInverse(Matrix m)
{
	m.a[0][3] *= -1;
	m.a[1][3] *= -1;
	m.a[2][3] *= -1;
	return m;
}

void MatrixPrint(Matrix m)
{
	cout << "Matrix:" << endl;
	for (int i = 0; i < MATRIX_SIZE; i++)
	{
		cout << "[" << m.a[i][0];
		for (int j = 1; j < MATRIX_SIZE; j++)
		{
			cout << ", " << m.a[i][j];
		}
		cout << "]" << endl;
	}
}

double GaussianFunction(double a, double b, double c, double x)
{
	return a * exp(-(x - b)*(x - b) / (2 * c*c));
}

double GaussianFunction(double sigma, double x)
{
	return 1.0 / (sqrt(2 * PI)*sigma) * exp(-x * x / (2 * sigma*sigma));
}

double getLength(Vec2f in2f)
{
	return sqrt(in2f[0] * in2f[0] + in2f[1] * in2f[1]);
}

double getLength(Vec3f in3f)
{
	return sqrt(in3f[0] * in3f[0] + in3f[1] * in3f[1] + in3f[2] * in3f[2]);
}

double getDistance(Vec3b in1, Vec3b in2)
{
	Vec3d v1 = Vec3d(in1[0], in1[1], in1[2]);;
	Vec3d v2 = Vec3d(in2[0], in2[1], in2[2]);
	return sqrt((v1[0] - v2[0])*(v1[0] - v2[0]) + (v1[1] - v2[1])*(v1[1] - v2[1]) + (v1[2] - v2[2]) * (v1[2] - v2[2]));
}

double getDistance(Vec2f in1, Vec2f in2)
{
	return sqrt((in1[0] - in2[0])*(in1[0] - in2[0]) + (in1[1] - in2[1])*(in1[1] - in2[1]));
}

Vec2f getRotateLeft(Vec2f in2f)
{
	return Vec2f(-in2f[1], in2f[0]);
}

Vec2f getRotateRight(Vec2f in2f)
{
	return Vec2f(in2f[1], -in2f[0]);
}

pair<int, int> getNextPoint(Vec2f in2f)
{
	in2f = normalize(in2f);
	double tmp = sqrt(0.5);
	int x, y;
	if (in2f[0] < -tmp)
		x = -1;
	else if (in2f[0] <= tmp)
		x = 0;
	else
		x = 1;
	if (in2f[1] < -tmp)
		y = -1;
	else if (in2f[1] <= tmp)
		y = 0;
	else
		y = 1;
	return make_pair(x, y);
}

Vec2f getNextPointBias(Vec2f in2f)
{
	in2f = normalize(in2f);
	double tmp = 0.5;
	int x, y;
	if (in2f[0] < -tmp)
		x = -1;
	else if (in2f[0] <= tmp)
		x = 0;
	else
		x = 1;
	if (in2f[1] < -tmp)
		y = -1;
	else if (in2f[1] <= tmp)
		y = 0;
	else
		y = 1;
	return Vec2f(x, y);
}

Vec4f EigenFromTensor(Vec4f in4f)
{
	bool bMaxLambda = false;
	float exmag = 2.0;
	float sqrtVal = sqrt((in4f[0] - in4f[3])*(in4f[0] - in4f[3]) + 4.0*in4f[1]*in4f[1]);
	//sqrtVal=dx'*dx'+dy'*dy'
	Vec2f lambda = Vec2f((in4f[0] + in4f[3] + sqrtVal) / 2.0, (in4f[0] + in4f[3] - sqrtVal) / 2.0);
	//lambda=(x*x+y*y,)

	Vec2f newdirt = (in4f[0]) < (in4f[3]) ? Vec2f(in4f[1], lambda[0] - in4f[0]) : Vec2f(lambda[0] - in4f[3], in4f[1]);
	if (abs(in4f[1]) <= 0.00)
		newdirt = abs(in4f[0]) < abs(in4f[3]) ? Vec2f(0.0, lambda[0] - in4f[0]) : Vec2f(lambda[0] - in4f[3], 0.0);

	float coh = 1.0 - (lambda[0] - lambda[1])*(lambda[0] - lambda[1]) / ((lambda[0] + lambda[1])*(lambda[0] + lambda[1]));
	float mag = pow((bMaxLambda ? max(abs(lambda[0]), abs(lambda[1])) : abs(lambda[0] + lambda[1])), exmag / 2.0);
	Vec2f tmp = getLength(newdirt) <= 0.0000001 ? normalize(Vec2f(1.0)) : normalize(newdirt);
	return  Vec4f(tmp[0], tmp[1], coh, mag);
}

Vec4f TensorFromEigen(Vec4f in4f)
{
	return Vec4f(in4f[0] * in4f[0] * in4f[3] * in4f[3],
		in4f[0] * in4f[1] * in4f[3] * in4f[3],
		in4f[0] * in4f[1] * in4f[3] * in4f[3],
		in4f[1] * in4f[1] * in4f[3] * in4f[3]);
}

cv::Mat thinImage(const cv::Mat & src, const int maxIterations = -1)
{
	assert(src.type() == CV_8UC1);
	cv::Mat dst;
	int width = src.cols;
	int height = src.rows;
	threshold(src, dst, 128, 1, cv::THRESH_BINARY);
	//src.copyTo(dst);
	int count = 0; //记录迭代次数
	while (true)
	{
		count++;
		if (maxIterations != -1 && count > maxIterations) //限制次数并且迭代次数到达
			break;
		std::vector<uchar *> mFlag; //用于标记需要删除的点
		//对点标记
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记
				//  p9 p2 p3
				//  p8 p1 p4
				//  p7 p6 p5
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);
				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p6 == 0 && p4 * p6 * p8 == 0)
					{
						//标记
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//直到没有点满足，算法结束
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空
		}

		//对点标记
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记
				//  p9 p2 p3
				//  p8 p1 p4
				//  p7 p6 p5
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);

				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p8 == 0 && p2 * p6 * p8 == 0)
					{
						//标记
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//直到没有点满足，算法结束
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空
		}
	}
	return dst * 255;
}


