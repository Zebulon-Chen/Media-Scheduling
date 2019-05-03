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

bool out = true;
bool input = false;
bool out_losses = true;
double avg = 0;

struct seg
{
	int beg, last, k;
	bool operator<(const seg& x) const {
		return last != x.last ? last < x.last : beg < x.beg;
	}
	bool operator>(const seg& x) const {
		return last != x.last ? last > x.last : beg > x.beg;
	}
};

void perm(int k, long long &min_loss, vector<int> &idxs, vector<int> &best_perm,
	const vector<seg> &segs,set<long long> &losses)
{
	if (k == idxs.size() - 1) {
		int curr_time = 0;
		long long loss = 0;
		for (auto idx : idxs) {
			if (curr_time <= segs[idx].beg) {
				curr_time = segs[idx].beg + segs[idx].last;
			}
			else {
				loss += (curr_time - segs[idx].beg)*segs[idx].k;
				curr_time += segs[idx].last;
			}
		}
		losses.insert(loss);
		if (loss < min_loss) {
			min_loss = loss;
			best_perm = idxs;
		}
		return;
	}
	for (int i = k;i < idxs.size();i++) {
		swap(idxs[k], idxs[i]);
		perm(k + 1, min_loss, idxs, best_perm, segs,losses);
		swap(idxs[k], idxs[i]);
	}
}

//复杂度O(n*n!)
long long check(const vector<seg> &segs) {
	long long min_loss = LONG_MAX;
	vector<int> idxs(segs.size()), best_perm;
	int ii = 0;
	generate(idxs.begin(), idxs.end(), [&]()->int {return ii++;});
	set<long long> losses;
	perm(0, min_loss, idxs, best_perm, segs,losses);
	if (out_losses) {
		cout << "first three losses:\n";
		int cnt = 1;
		for (auto it = losses.begin();it != losses.end();it++) {
			cout << *it << " ";
			if (++cnt > 15)
				break;
		}
		cout << endl;
	}
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
	vector<int> kks = { 1,2,3,4,5,6 };
	vector<seg> segs(n);
	if (input) {
		f(i, 0, n) {
			cin >> segs[i].beg >> segs[i].last >> segs[i].k;
		}
	}
	else {
		f(i, 0, n) {
			segs[i].beg = rand() % lim;
			do {
				segs[i].last = rand() % (lim / 5);
			} while (segs[i].last == 0);
			segs[i].k = kks[rand() % kks.size()];
		}
	}
	//按照到达时间先后顺序排序，若同时到达，则先处理
	sort(all(segs), [](seg& a, seg& b) {
		return a.beg != b.beg ? a.beg < b.beg : a.last/a.k < b.last/b.k;});
	segs[0].beg = 0;
	if (out) {
		cout << "segments:\n";
		for (auto s : segs)
			cout << s.beg << "\t" << s.last <<"\t"<< s.k << endl;
		cout << "-------------\narrive\tserve\twait\tfinish\n";
	}
	vector<int> order;
	deque<seg> q;
	int curr_idx = 0, curr_time = 0;
	long long total_loss = 0;
	while (curr_idx < n || !q.empty()) {
		//当所有任务都已入队，直接处理队中任务
		if (curr_idx == n) {
			//计算队中所有k之和
			int k_sum = 0;
			for (auto &s : q) {
				k_sum += s.k;
			}
			while (!q.empty()) {
				//比较选择哪个任务可以带来最小的*新增*损失
				int min_loss = INT_MAX;
				deque<seg>::iterator min_it = q.begin();//换成auto也行
				for (auto it = q.begin();it != q.end();it++) {
					int loss = it->last*(k_sum - it->k);
					if ( loss< min_loss) {
						min_it = it;
						min_loss = loss;
					}
				}
				order.push_back(min_it->beg);
				total_loss += min_loss;
				curr_time += min_it->last;
				k_sum -= min_it->k;
				q.erase(min_it);
			}
			break;
		}
		//系统初始情况或者期间没有任务及时到来
		//推进当前时间，相当于指针在时间轴往前跑，直到找到一个新的任务
		//while用来将同时刻的任务全部入队
		if (q.empty()) {
			int time = segs[curr_idx].beg;
			while (curr_idx<n&&segs[curr_idx].beg == time)
				q.push_back(segs[curr_idx++]);
			curr_time = q.front().beg;
		}
		int k_sum = 0;
		for (auto &s : q) {
			k_sum += s.k;
		}
		//比较选择哪个队中任务可以带来最小的*新增*损失
		int min_loss = INT_MAX;
		deque<seg>::iterator min_it = q.begin();//换成auto也行
		for (auto it = q.begin();it != q.end();it++) {
			int loss = it->last*(k_sum - it->k);//若选当前任务，当其执行完毕时的队列其他任务新增损失
			int idx = curr_idx;
			//区间内到达任务由于目前在处理it任务而造成的新增损失
			while (idx < n&&segs[idx].beg <= curr_time + it->last) {
				loss += (curr_time + it->last - segs[idx].beg)*segs[idx].k;
				idx++;
			}
			if (loss < min_loss) {
				min_loss = loss;
				min_it = it;
			}
		}
		//开始比较是否可以先执行在后续区间内的任务能带来更小的损失
		int lower_idx = -1;
		for (int i = curr_idx;i < n&&segs[i].beg <= curr_time + min_it->last;i++) {
			//队列中所有任务由于继续等待造成的新增损失
			int end_time = segs[i].beg + segs[i].last;
			int loss = (end_time - curr_time)*k_sum;
			for (int j = curr_idx;j < n&&segs[j].beg <= end_time;j++) {
				if(j!=i)
					loss += (end_time - segs[j].beg)*segs[j].k;
			}
			if (loss < min_loss) {
				min_loss = loss;
				lower_idx = i;
			}
		}
		total_loss += min_loss;
		//区间内没有可以造成更小损失的任务，执行队内任务
		if (lower_idx==-1) {
			curr_time += min_it->last;
			order.push_back(min_it->beg);
			q.erase(min_it);
			while (curr_idx < n&&segs[curr_idx].beg <= curr_time) {
				q.push_back(segs[curr_idx]);
				curr_idx++;
			}
		}
		else {
			order.push_back(segs[lower_idx].beg);
			curr_time = segs[lower_idx].beg + segs[lower_idx].last;
			//将该任务前的到达任务入队，并注意跳过该任务
			while (curr_idx < n&&segs[curr_idx].beg <= curr_time) {
				if (curr_idx != lower_idx)
					q.push_back(segs[curr_idx]);
				curr_idx++;
			}
		}
	}

	if (out)
		cout << "-------------\n" << total_loss << endl;
	if (out)
		copy(all(order), ostream_iterator<int>(cout, " "));
	cout << endl;
	long long ans = check(segs);
	printf("n=%d, total_loss: %lld, ans: %lld\n", n, total_loss, ans);
	if (ans == 0)
		cout << "no wait time\n";
	else {
		double bias = (double)((total_loss - ans)*1.0) / (ans*1.0) * 100;
		cout << "bias: " << setprecision(4) << bias << "%" << endl;
		avg += bias;
	}
}


int main()
{
	srand((unsigned)time(NULL));
	vector<int> nns = { 3,5,6,7,8 };
	scheduling(8);
	//int num = 20;
	//f(i, 0, nns.size()) {
	//	f(j, 0, num) {
	//		scheduling(nns[i]);
	//	}
	//}
	//cout << "avg:" << avg / (num*nns.size()) << endl;

	system("pause");
	return 0;
}