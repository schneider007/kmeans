#include <string>
#include <vector>
#include <set>
#include <numeric>
#include <algorithm>
#include <stdio.h>
#include <fstream>

using namespace std;

class Point2D
{
public:
	Point2D():x_(0),y_(0){}
	Point2D(float x,float y):x_(x),y_(y){}

	Point2D& operator +=(const Point2D& rhs)
	{
		x_ += rhs.x_;
		y_ += rhs.y_;
		return *this;
	}
	Point2D operator /(int num)
	{
		x_ /= num;
		y_ /= num;
		return *this;
	}
	bool operator !=(const Point2D& rhs)
	{
		return x_ != rhs.x_ || y_ != rhs.y_;
	}

	float x_;
	float y_;
};

double dist2(const Point2D& a, const Point2D& b)
{
	return (b.x_ - a.x_)*(b.x_ - a.x_) + (b.y_ - a.y_)*(b.y_ - a.y_);
}

int main()
{
	vector<Point2D> pts;
	for (int i = 0; i < 50; ++i)
	{
		for (int j = 0; j < 50; ++j)
		{
			pts.push_back(Point2D(i, j));
		}
	}
	int classNr = 8;
	int k = std::min(classNr,(int)pts.size());//类别数
	vector<vector<int>> group_idx(k);//保存分类后点的序号
	vector<Point2D> center(k);

	//初始化类的中心
	/*for (int i = 0; i < k; ++i)
		center[i] = pts[i];*/

	//类中心随机初始化
	std::set<int> centerIdx;
	while (centerIdx.size() < k)//生成相同的随机整数也是有可能的
		centerIdx.insert(rand() % (pts.size() + 1));
	int id = 0;
	for (auto& i : centerIdx)
	{
		center[id] = pts[i];
		id++;
	}

	int iter_nr = 0;
	while (true)
	{
		for (auto& group : group_idx)
			group.clear();

		for (int i=0;i<pts.size();++i)
		{
			double min_dist2 = numeric_limits<double>::max();
			int min_group_idx = -1;
			double dist2_tmp;
			for (int j = 0; j < group_idx.size(); ++j)
			{
				dist2_tmp = dist2(pts[i], center[j]);
				if (min_dist2 > dist2_tmp)
				{
					min_dist2 = dist2_tmp;
					min_group_idx = j;
				}
			}
			group_idx[min_group_idx].push_back(i);
		}

		//聚类过程中，某一类的点完全被划分到其它类中时，删除该类
		for (int i = group_idx.size() - 1; i >= 0; --i)
		{
			if (group_idx[i].empty())
			{
				group_idx.erase(group_idx.begin() + i);
				center.erase(center.begin() + i);
			}
		}

		vector<Point2D> prev_center = center;

		for (int i = 0; i < group_idx.size(); ++i)
		{
			Point2D sum;
			for (auto& idx : group_idx[i])
				sum += pts[idx];
			center[i] = sum / group_idx[i].size();
		}
		bool same = true;
		for (int i = 0; i < group_idx.size(); ++i)
		{
			if (center[i] != prev_center[i])
			{
				same = false;
				break;
			}
		}
		iter_nr++;
		if (same || iter_nr > 500)
			break;
	}

	ofstream out;

	out.open("center.txt");
	for (auto& pt : center)
		out << pt.x_ << " " << pt.y_ << endl;
	out.close();

	for (int i = 0; i < group_idx.size(); ++i)
	{
		char file_name[100];
		sprintf_s(file_name,sizeof(file_name), "class%d.txt", i + 1);
		out.open(file_name);
		for (auto& idx : group_idx[i])
		{
			out << pts[idx].x_ << " " << pts[idx].y_ << endl;
		}
		out.close();
	}
}