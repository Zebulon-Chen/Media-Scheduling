#include <cstdio>
#include <cmath>
#include <climits>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iomanip>
#include <iterator>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#define f(i,a,b) for(int i=a;i<b;i++)
#define all(a) a.begin(), a.end()
using namespace std;

bool out = false;
bool input = false;

struct seg
{
	int beg, last;
	bool operator<(const seg& x) const {
		return last != x.last ? last < x.last : beg < x.beg;
	}
	bool operator>(const seg& x) const {
		return last != x.last ? last > x.last : beg > x.beg;
	}
};

void perm(int k, long long &min_loss, vector<int> &idxs, vector<int> &best_perm, const vector<seg> &segs)
{
	if (k == idxs.size() - 1) {
		int curr_time = 0;
		long long loss = 0;
		for (auto idx : idxs) {
			if (curr_time <= segs[idx].beg) {
				curr_time = segs[idx].beg + segs[idx].last;
			}
			else {
				loss += curr_time - segs[idx].beg;
				curr_time += segs[idx].last;
			}
		}
		if (loss < min_loss) {
			min_loss = loss;
			best_perm = idxs;
		}
		return;
	}
	for (int i = k;i < idxs.size();i++) {
		swap(idxs[k], idxs[i]);
		perm(k + 1, min_loss, idxs, best_perm, segs);
		swap(idxs[k], idxs[i]);
	}
}

//复杂度O(n*n!)
long long check(const vector<seg> &segs) {
	long long min_loss = LONG_MAX;
	vector<int> idxs(segs.size()), best_perm;
	int ii = 0;
	generate(idxs.begin(), idxs.end(), [&]()->int {return ii++;});
	perm(0, min_loss, idxs, best_perm, segs);
	if (out) {
		cout << "Perm ans:\nmin_loss:" << min_loss << "\njob order:\n";
		for (auto idx : best_perm) {
			cout << segs[idx].beg << '\t' << segs[idx].last << endl;
		}
	}
	return min_loss;
}

void scheduling(int n) {
	const int lim = 100;
	vector<seg> segs(n);
	if (input) {
		vector<int> order(n);
		f(i, 0, n) {
			cin >> segs[i].beg >> segs[i].last;
		}
	}
	else {
		f(i, 0, n) {
			segs[i].beg = rand() % lim;
			do {
				segs[i].last = rand() % (lim / 5);
			} while (segs[i].last == 0);
		}
	}

	sort(all(segs), [](seg& a, seg& b) {
		return a.beg != b.beg ? a.beg < b.beg : a.last < b.last;});
	segs[0].beg = 0;
	if (out) {
		cout << "segments:\n";
		for (auto s : segs)
			cout << s.beg << "\t" << s.last << endl;
		cout << "-------------\narrive\tserve\twait\tfinish\n";
	}

	//小根堆
	priority_queue<seg, vector<seg>, greater<seg>> q;
	int idx = 0;
	int curr_time = 0;
	long long total_loss = 0;
	while (idx < n || !q.empty()) {
		//任务已经全部到达，处理队列中剩余任务
		if (idx == n) {
			while (!q.empty()) {
				if (out)
					cout << q.top().beg << '\t' << curr_time << '\t' << curr_time - q.top().beg << '\t';
				total_loss += curr_time - q.top().beg;
				curr_time += q.top().last;
				if (out)
					cout << curr_time << endl;
				q.pop();
			}
			break;
		}
		//初始情况或者中途没有任务及时到来，系统空置一段时间
		//直接将当前idx入队，准备与后续比较
		if (q.empty()) {
			q.push(segs[idx++]);
			curr_time = q.top().beg;
		}
		//当前队列中最短作业持续时间
		int lk = q.top().last;
		vector<int> bp_idxs;//记录符合条件的idx
							//int beg_idx = idx;//记录区间的初始idx
		int ii = idx;//从当前idx开始游走
		while (ii < n&&segs[ii].beg <= curr_time + lk) {
			if (segs[ii].beg + segs[ii].last <= curr_time + lk) {
				bp_idxs.push_back(ii);
			}
			ii++;
		}
		//没有符合条件的任务，包括有开始时间在区间内但是结束时间在区间外的任务
		//执行堆顶任务，并将所有区间内到达的任务入队
		if (bp_idxs.empty()) {
			//是先前队列空了直接入队的任务
			//curr_time>=q.top().beg
			//刚才如果有直接入队的则新增loss为0
			total_loss += curr_time - q.top().beg;
			if (out)
				cout << q.top().beg << '\t' << curr_time << '\t' << curr_time - q.top().beg << '\t';
			curr_time += lk;
			if (out)
				cout << curr_time << endl;
			q.pop();
			//区间内到达的任务入队
			while (idx < ii) {
				q.push(segs[idx++]);
			}
		}
		else {
			//有符合条件的任务
			//比较各个loss之间的最小值
			//priority_queue不支持堆内元素遍历，后期考虑set
			priority_queue<seg, vector<seg>, greater<seg>> qq(q);
			//0号位为队列中的top的loss
			vector<int> loss(1 + bp_idxs.size(), 0);
			//倘若执行堆顶任务带来的loss
			int bp_loss = loss[0] = curr_time - qq.top().beg;
			qq.pop();
			//堆中任务在C+lk时刻的损失总和
			while (!qq.empty()) {
				loss[0] += curr_time + lk - qq.top().beg;
				bp_loss += curr_time - qq.top().beg;
				qq.pop();
			}
			//c到c+lk时间内新增任务
			for (int i = idx;i < ii;i++) {
				loss[0] += curr_time + lk - segs[i].beg;
			}
			//计算c到c+lk区间内符合条件的loss
			for (int i = 0;i < bp_idxs.size();i++) {
				int end_time = segs[bp_idxs[i]].beg + segs[bp_idxs[i]].last;
				loss[i + 1] = bp_loss + q.size()*(end_time - curr_time);
				for (int j = idx;j < n&&segs[j].beg <= end_time;j++) {
					loss[i + 1] += end_time - segs[j].beg;
				}
				//该任务自身的服务时间被多算了一次
				loss[i + 1] -= segs[bp_idxs[i]].last;
			}
			int min_idx = distance(loss.begin(), min_element(loss.begin(), loss.end()));
			//还是选堆顶的
			if (min_idx == 0) {
				total_loss += curr_time - q.top().beg;
				if (out)
					cout << q.top().beg << '\t' << curr_time << '\t' << curr_time - q.top().beg << '\t';
				curr_time += lk;
				if (out)
					cout << curr_time << endl;
				q.pop();
				while (idx < ii) {
					q.push(segs[idx++]);
				}
			}
			else {
				int bp_idx = bp_idxs[min_idx - 1];
				if (out)
					cout << segs[bp_idx].beg << '\t' << segs[bp_idx].beg << '\t' << 0 << '\t';
				curr_time = segs[bp_idx].beg + segs[bp_idx].last;
				if (out)
					cout << curr_time << endl;
				while (idx < n&&segs[idx].beg <= curr_time) {
					if (idx != bp_idx)
						q.push(segs[idx]);
					idx++;
				}
			}
		}
	}
	if (out)
		cout << "-------------\n" << total_loss << endl;
	long long ans = check(segs);
	printf("n=%d, total_loss: %lld, ans: %lld\n", n, total_loss, ans);
	if (ans == 0)
		cout << "no wait time\n";
	else
		cout << "bias: " <<setprecision(4)<< (double)((total_loss - ans)*1.0) / (ans*1.0)*100<<"%" << endl;
}


int main()
{
	srand((unsigned)time(NULL));
	vector<int> nns = { 3,5,8,10 };
	//scheduling(8);
	f(i, 0, nns.size()) {
		f(j, 0, 3) {
			scheduling(nns[i]);
		}
	}

	system("pause");
	return 0;
}