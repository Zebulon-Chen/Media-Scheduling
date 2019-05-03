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

//���Ӷ�O(n*n!)
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
	//���յ���ʱ���Ⱥ�˳��������ͬʱ������ȴ���
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
		//��������������ӣ�ֱ�Ӵ����������
		if (curr_idx == n) {
			//�����������k֮��
			int k_sum = 0;
			for (auto &s : q) {
				k_sum += s.k;
			}
			while (!q.empty()) {
				//�Ƚ�ѡ���ĸ�������Դ�����С��*����*��ʧ
				int min_loss = INT_MAX;
				deque<seg>::iterator min_it = q.begin();//����autoҲ��
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
		//ϵͳ��ʼ��������ڼ�û������ʱ����
		//�ƽ���ǰʱ�䣬�൱��ָ����ʱ������ǰ�ܣ�ֱ���ҵ�һ���µ�����
		//while������ͬʱ�̵�����ȫ�����
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
		//�Ƚ�ѡ���ĸ�����������Դ�����С��*����*��ʧ
		int min_loss = INT_MAX;
		deque<seg>::iterator min_it = q.begin();//����autoҲ��
		for (auto it = q.begin();it != q.end();it++) {
			int loss = it->last*(k_sum - it->k);//��ѡ��ǰ���񣬵���ִ�����ʱ�Ķ�����������������ʧ
			int idx = curr_idx;
			//�����ڵ�����������Ŀǰ�ڴ���it�������ɵ�������ʧ
			while (idx < n&&segs[idx].beg <= curr_time + it->last) {
				loss += (curr_time + it->last - segs[idx].beg)*segs[idx].k;
				idx++;
			}
			if (loss < min_loss) {
				min_loss = loss;
				min_it = it;
			}
		}
		//��ʼ�Ƚ��Ƿ������ִ���ں��������ڵ������ܴ�����С����ʧ
		int lower_idx = -1;
		for (int i = curr_idx;i < n&&segs[i].beg <= curr_time + min_it->last;i++) {
			//�����������������ڼ����ȴ���ɵ�������ʧ
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
		//������û�п�����ɸ�С��ʧ������ִ�ж�������
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
			//��������ǰ�ĵ���������ӣ���ע������������
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