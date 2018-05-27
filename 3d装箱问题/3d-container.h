/* by cjf 2017/07*/
#include<stdio.h>
#include<string.h>
#include<algorithm>
#include<vector>
#include<stack>
#include<time.h>
#include<set>
#include<queue>
#include<map>
#include<windows.h>
using namespace std;
//#define DEBUG
#define HASH
#define MAXTYPE 101                           //����box���������±��1��ʼ
#define GETBRANCH(d,e) (int(pow(1.0*e,1.0/d)))
/*****  ���� *********/
bool   C2_CONSTRAIN    =false;           //�Ƿ�����C2Լ��
bool   GenGeneralBlock =false;           //�Ƿ����ɸ��Ͽ�
int    MaxLevel        =2;               //���Ͽ��������
double MinAreaRate     =0.96;             //���Ͽ鶥�������С�ٷֱ�
double MinFillRate     =0.98;             //���Ͽ���С�Ŀռ�ʹ����
int    MaxBlocks       =10000;            //���Ͽ�����������ʵ���У�����΢������
int    MaxHeap         =16;                //HEAP�����Ԫ��
int    MaxLayer        =6;
int    LayerExt        =8;               //��ǰlayerĳ��state�����չ�Ľڵ�
int    MaxDepth        =2;                //dfs���������
int    MaxSearchEffort =1<<8;            //

struct CUBOID{                               //������
  int  x,y,z;
  int getVolumn(){return x*y*z;};
  CUBOID(int x=0,int y=0,int z=0):x(x),y(y),z(z){}
  bool contain(CUBOID* p){ return x>=p->x && y>=p->y && z>=p->z; }
  bool contain(CUBOID  c){ return x>=c.x && y>=c.y && z>=c.z; }
  int  display(){ printf("%d * %d * %d =%d\n",x,y,z,x*y*z);}
};
struct POINT3{
  int x,y,z;
  POINT3(int x=0,int y=0,int z=0):x(x),y(y),z(z){}
  POINT3(POINT3 p,CUBOID l){ //
    x=p.x+l.x;
    y=p.y+l.y;
    z=p.z+l.z;
  }
  void display(){printf("(%d,%d,%d)\n",x,y,z);}
};
POINT3 operator + (POINT3 p,CUBOID c){return POINT3(p,c);}
struct BR_INPUT{                             //һ��BR����
  CUBOID container;
  int nType;                                 //�±��1��ʼ
  int l[MAXTYPE];                            //ÿ�����Ӱ���w>l>h����
  int w[MAXTYPE];
  int h[MAXTYPE];
  int il[MAXTYPE];
  int iw[MAXTYPE];
  int ih[MAXTYPE];
  int nNum[MAXTYPE];
  void read();
  double solution_verify(vector<CUBOID>& box,vector<POINT3>& pos);
  void   output_strategy(vector<CUBOID>& box,vector<POINT3>& pos){  //
    printf("------------------���ӷ��ò���[��%d������]------------------------\n",box.size());
    //���box����λ��
    for (int i=0;i<box.size();i++){
      CUBOID t=box[i];
      POINT3 p=pos[i];
      printf("[BLOCK %d] ��������(%3d,%3d,%3d) ���Ӵ�С %d*%d*%d \n",i,p.x,p.y,p.z,t.x,t.y,t.z);
    }
  }

};
struct BLOCK{
  CUBOID cEnvelop;                           //BLOCK��ӳ�����
  int    nBoxVolumn;                         //BLOCK�ڲ�Box�����С
  int    nBoxNum[MAXTYPE];                    //BLOCK�ڲ�Boxĳ���͵ĸ���
  int    nType;                              //0��ʾSimple Block�� 1��2��3�ֱ����x,y,z���
  int    isSimple;                           //�Ƿ�ΪSimple Block������ǣ���¼��box type
  BLOCK* bl1;
  BLOCK* bl2;                                //��ΪGeneral Block������bl1��bl2ͨ��nType��ʽ���
  //
  int    nUpperX;
  int    nUpperY;                            //������C2Լ��ʱ����¼Block�ϲ������
  int    hash;                               //�����ж�����block�Ƿ�һ��
  int    initHash();
  //���캯��
  BLOCK(){}
  BLOCK(int x,int y,int z,int boxType,int boxNum);  //Simple Block
  BLOCK(BLOCK* b1,BLOCK* b2,int type);              //General Block������������BLOCK������������Լ������(������������������̫��)
  //����
  bool isGeneralBlock();
  int  decompose(POINT3 blkPos,vector<CUBOID>& boxType,vector<POINT3>& boxPos,BR_INPUT& input); //�ֽ⵱ǰblock����box����boxType,boxPos
  int  display();
};


struct STATE{                                //�����е����ݽṹ
  vector<BLOCK*> pbl;
  vector<POINT3> pbl_pos;
  int            vol;                          //Volumn
  int            Bres[MAXTYPE];              //Block Residual
  stack<CUBOID>  Rs_stack;                   //Residual Space Stack
  stack<POINT3>  Rs_stack_pos;
  bool           isComplete;                 //�Ƿ�ﵽ����״̬
  //�洢���յ�solution

  STATE(){}
  STATE(int *nBoxNum,CUBOID* container);
  STATE(STATE *state);                       //����һ��
  const bool operator < (STATE  s2)const{
    return vol> s2.vol;
  }
  void display(){
      printf("--STATE--\n");
    printf("  usage:%d\n",vol);
    printf("  BLOCK:\n");
    for (int i=0;i<pbl.size();i++) printf("  "),pbl[i]->cEnvelop.display();
  }
};

struct HEAP{
  vector<STATE*> s;
  vector<int>    v; //s����complete������
  int n;            //��ౣ��n������STATE
  HEAP(){}
  HEAP(int n):n(n){}

  int    size(){return s.size();}
  void   push(STATE* state,int volumn);
  void   clear(){ for (int i=0;i<s.size();i++) delete s[i]; s.clear();v.clear();}
  STATE* get(int idx){return s[idx];}
};
//MLHS��
//ֱ�ӵ���work���������ؿռ�ʹ���ʡ�
//mlhs.sol_box��mlhs.solboxPos ��¼box�ķ���λ�á�
//solution_verify������֤sol_box��solbox_Pos�Ƿ���ȷ��
class MLHS{
private:
  /****** �㷨���ݽṹ *********/
  CUBOID           container;                             //����box�Ŀռ�
  vector<BLOCK*>   blockTable;                            //���ɵ�block
  int              nType;                                 //����box����
  int              nBoxNum[MAXTYPE];                      //����Box����
  set<int>         block_hash;                            //block���ء���ֹ����һ����block
  STATE*           state;                                 //�����̵�״̬
  /****** ����  ************/
  bool             isBlockValid(BLOCK* block);           //�ж�General Block�Ƿ�������������
  int              init(BR_INPUT& input);                //����һ������input����ʼ��context
  int              genSimpleBlock();                     //����simple block��
  int              genGeneralBlock();
  static bool      cmp_block_table(BLOCK *x,BLOCK *y){return x->cEnvelop.getVolumn() > y->cEnvelop.getVolumn();} //��������blocks
  static bool      cmp_rs(CUBOID x,CUBOID y){return x.getVolumn() > y.getVolumn();}
  //���º�������װ��block
  int              insertBlock(BLOCK *block);
  int              placeBlock(STATE *state,BLOCK* block); //��block����state��ջ����
  int              removeBlock(STATE *state,BLOCK* block,CUBOID rs,POINT3 rs_pos);//�ָ���������

  //int              addBlock(STATE *state,BLOCK* block);  //��block���뵱ǰstate�С�������residual space
  int              completeState(STATE *state);       //����state���Դﵽ��volumn����̰�ģ�
  BLOCK*           findNextBlock(STATE* state,int search_effort); //MyltiLayerSearch���ҵ����ʺ�state����һ��block�������ڷ���NULL
  int              generateRSBlockList(STATE*state,CUBOID rs,POINT3 rs_pos,int mxBlocks,vector<BLOCK*>* bll); //��state�У��ҳ�����rs��block������bll
  //curRs ��ȥ block������3������� state�С� blockList���Է���curRs����������и
  int              generateDaughterSpaces(STATE* state,CUBOID curRs,POINT3 curRs_pos,BLOCK *block,vector<BLOCK*>* blockList);
  bool             hasEnoughBox(STATE* state,BLOCK* block);     //state���Ƿ�����㹻��box
  //��state�������������Ϊd�����Ϊbranch������ջ��rs���ŵ�block��������������complete_state�� �������������ֵ��
  //��;�з���ʱӦ��֤state�����޸ģ�
  int              dfs(STATE* state,int depth,int branch);
  int              accelerate(); //�Ż�
public:
  MLHS(){state=NULL;}
  //solution
  vector<CUBOID>   sol_box;
  vector<POINT3>   sol_boxPos;
  double           Work(BR_INPUT &input,bool bGeneral); //����ʹ����
  //int              Work2(BR_INPUT &input,bool bGeneral);
};

