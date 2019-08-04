#include "solver.h"
#include <limits.h>

double getDistance(Point p1, Point p2){
	double delta_x = p1.x - p2.x;
	double delta_y = p1.y - p2.y;

	return sqrt(delta_x * delta_x + delta_y * delta_y);
}


void bottomUp(Instance &instance, int sol[][M]){
	 double dp[1 << (instance.n - 2)][instance.n - 2];

	 for(int s = 1; s < 1 << (instance.n - 2); s++){
	      for(int i = 1; i < instance.n - 1; i++){
	           dp[s][i - 1] = double(INT_MAX);
	      }
	 }

	 for(int i=1; i < instance.n-1; i++){
	 	   dp[1 << (i - 1)][i - 1] = getDistance(instance.points[0], instance.points[i]);
	 	   sol[1 << (i - 1)][i-1] = 0;
	 }

	 for (int s = 1; s <= ((1 << (instance.n-2)) - 1); s++){
	      bitset<M> b(s);
	      for (int i=1; i < instance.n-1; i++ ){
	           if (b[i-1] == 1){
	                int parent = 0;
	                double min = dp[b.to_ulong()][i-1];
	                for (int j=1; j < instance.n-1; j++){
	                     if(b[j-1] == 1 && j != i){
	                          bitset<M> b2 = b;
	                          b2[i-1] = 0;
	                          if(min > dp[b2.to_ulong()][j-1] + getDistance(instance.points[i], instance.points[j])){
									min = dp[b2.to_ulong()][j-1] + getDistance(instance.points[i], instance.points[j]);
									parent = j;
	                          }
	                     }
	                }
	                sol[b.to_ulong()][i-1] = parent;
	                dp[b.to_ulong()][i-1] = min;
	           }
	      }
	 }

	 double min_distance = double(INT_MAX);
	 int prev = 0;
	 for(int i=1; i<instance.n-1; i++){
	      if(dp[(1 << (instance.n - 2)) - 1][i - 1] + getDistance(instance.points[i], instance.points[instance.n-1]) < min_distance){
	           min_distance = dp[(1 << (instance.n - 2)) - 1][i - 1] + getDistance(instance.points[i], instance.points[instance.n-1]);
	           prev = i;
	      }
	 }

	sol[(1 << (instance.n - 2)) - 1][instance.n-1] = prev;
}


vector<int> solveBottomUp(Instance &instance, int timelimit, chrono::high_resolution_clock::time_point &started){
	int sol[1 << (instance.n - 2)][M];

    bottomUp(instance, sol);

	auto finished = chrono::high_resolution_clock::now();
	
	//cout << "Time difference = " << chrono::duration_cast<chrono::microseconds>(finished - started).count() << endl;
	
	bitset<M> b((1 << (instance.n - 2)) - 1);
	vector<int> path;

	int curr = sol[b.to_ulong()][instance.n - 1];
	int parent = sol[b.to_ulong()][curr - 1];
	b.reset(curr - 1);

	while(parent != 0){
		path.push_back(curr);
		curr = parent;
		parent = sol[b.to_ulong()][parent - 1];
		b.reset(curr - 1);
	}

	path.push_back(curr);
	reverse(path.begin(), path.end());

     return path;
}


double topDown(Instance &instance, int mask, int pos, int visited_all, double dp[][M], int sol[][M]){

	if(mask == visited_all){
		sol[mask][pos] = instance.n-1;
		return getDistance(instance.points[pos], instance.points[instance.n-1]);
	}

	if(dp[mask][pos] != -1){
		return dp[mask][pos];
	}

	double ans = double(INT_MAX);
	int prev;
	for(int point=1; point< instance.n-1; point++){
		if ((mask&(1<<(point-1))) == 0){
			double new_ans = getDistance(instance.points[pos], instance.points[point]) + topDown(instance, mask|(1<<(point-1)), point, visited_all, dp, sol);
			if (new_ans < ans){
				ans = new_ans;
				prev = point;
			}

		}
	}


	sol[mask][pos] = prev;
	return dp[mask][pos] = ans;
}



vector<int> solveTopDown(Instance &instance, int timelimit, chrono::high_resolution_clock::time_point &started){
	int visited_all = (1<<(instance.n - 2)) - 1;
	int sol[1 << (instance.n - 2)][M];

	double dp[(1<<instance.n)][M];
	for(int i=0; i<(1<<instance.n); i++){
		for(int j=0; j<instance.n; j++){
			dp[i][j] = -1;
		}
	}

	double minimo = topDown(instance, 0, 0, visited_all, dp, sol);
	auto finished = chrono::high_resolution_clock::now();
	
	//cout << "Time difference = " << chrono::duration_cast<chrono::microseconds>(finished - started).count() << endl;

	bitset<M> b(0);
	vector<int> path;

	int curr = sol[0][0];
	b.set(curr - 1);
	int suc = sol[b.to_ulong()][curr];

	while(suc != instance.n - 1){
		path.push_back(curr);
		curr = suc;
		b.set(curr - 1);
		suc = sol[b.to_ulong()][curr];
	}
	path.push_back(curr);

	return path;
}
