#include<stdio.h>
#include<math.h>
#include<vector>
using namespace std;
#define GETBRANCH(d,e) (int(pow(1.0*e,1.0/d)))
struct PP{
  int a;
  int b[4];
};
int fun(){
  int a;
  int *b=new int();
  vector<int> v;
  v.push_back(1);
  printf("%x\n%x\n%x\n%x\n",&a,b,&v[0],&v);
  return 0;
}
int main(){
  fun();
  return 0;
}
