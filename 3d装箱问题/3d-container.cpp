#include<stdio.h>
#include<string.h>
#include<algorithm>
#include<vector>
#include<stack>
#include<time.h>
#include<set>
#include<queue>
#include<math.h>
#include<map>
#include<windows.h>
#include "3d-container.h"
using namespace std;
void BR_INPUT::read(){
    scanf("%d%d%d",&container.x,&container.y,&container.z);
    scanf("%d",&nType);
    if (nType>=MAXTYPE){printf("type=%d\n",nType);exit(-4);}
    for (int i=1;i<=nType;i++){
      int tmp;scanf("%d",&tmp);
      scanf("%d %d %d %d %d %d %d",&l[i],&il[i],&w[i],&iw[i],&h[i],&ih[i],&nNum[i]);
      //����w>l>h����ð��
      if (w[i]<l[i]) swap(w[i],l[i]),swap(iw[i],il[i]);
      if (l[i]<h[i]) swap(l[i],h[i]),swap(il[i],ih[i]);
      if (w[i]<l[i]) swap(w[i],l[i]),swap(iw[i],il[i]);
    }
  }

short occupy[600][600][600];
double BR_INPUT::solution_verify(vector<CUBOID>& box,vector<POINT3>& pos){
     int totV=0;
     for (int i=0;i<container.x;i++)
       for (int j=0;j<container.y;j++)
         for (int k=0;k<container.z;k++)
           occupy[i][j][k]=0;
     for (int i=0;i<box.size();i++){
       CUBOID b=box[i];
       totV+=b.getVolumn();
       int len[3]={b.x,b.y,b.z};sort(len,len+3);
       //������ӵ�����
       int boxType=-1;
       for (int t=1;t<=nType;t++){
          if (w[t]==len[2] && l[t]==len[1] && h[t]==len[0] && nNum[t]>0) {boxType=t;break;}
       }
       //��ȥ������
       if (boxType==-1) {
           printf("No Box For %d*%d*%d\n",b.x,b.y,b.z);
           exit(11);
        }
       nNum[boxType]--;
       //�ռ䱻ռ���
       POINT3 boxPos=pos[i];
       if (boxPos.x+b.x>container.x || boxPos.y+b.y>container.y ||boxPos.z+b.z>container.z){
         printf("(%d,%d,%d)",boxPos.x+b.x,boxPos.y+b.y,boxPos.z+b.z);container.display();
         printf("x ,y,z�����");exit(12);
       }
       for (int px=boxPos.x;px<boxPos.x+b.x;px++)
         for (int py=boxPos.y;py<boxPos.y+b.y;py++)
           for (int pz=boxPos.z;pz<boxPos.z+b.z;pz++)
           if (occupy[px][py][pz]){
             printf("(%d,%d,%d) has been occupied! %d box\n",px,py,pz,i);
             exit(12);
           }else
           occupy[px][py][pz]=1;
     }
     return 1.0*totV/container.getVolumn();
}
void  HEAP::push(STATE* state,int volumn){
    //for (int i=0;i<v.size();i++) if (volumn==v[i])return;

    s.push_back(state);
    v.push_back(volumn);
    int k=s.size()-1;
    while (k>=1 && v[k]>v[k-1]) {
        swap(s[k],s[k-1]);
        swap(v[k],v[k-1]);
        k--;
    }
    while (s.size()>n) {
        delete s[s.size()-1];
        s.pop_back();
        v.pop_back();
    }
  }
int BLOCK::initHash(){
    #define MOD 1000000007
    hash=0;
    for (int i=1;i<MAXTYPE;i++) hash+=i*i*nBoxNum[i];
    if (C2_CONSTRAIN)
      hash=1ll*hash*(nUpperX*1000+nUpperY)%MOD+1;
    hash=1ll*hash*(cEnvelop.x*10000+cEnvelop.y*100+cEnvelop.z)%MOD+1;
    return 0;
}

BLOCK::BLOCK(int x,int y,int z,int boxType,int boxNum){                  //Simple Block
    cEnvelop.x=x;
    cEnvelop.y=y;
    cEnvelop.z=z;
    nBoxVolumn=x*y*z;
    nType=0;

    bl1=bl2=NULL;
    if (C2_CONSTRAIN){
      nUpperX=x;
      nUpperY=y;
    }else{
      nUpperX=nUpperY=0;
    }
    memset(nBoxNum,0,sizeof(nBoxNum));
    nBoxNum[boxType]=boxNum;
    isSimple=boxType;
    initHash();
  }
BLOCK::BLOCK(BLOCK* b1,BLOCK* b2,int type){                //General Block������������BLOCK������������Լ������(������������������̫��)
    bl1=b1;
    bl2=b2;
    nType=type;
    isSimple=0;
    nBoxVolumn=bl1->nBoxVolumn+bl2->nBoxVolumn;
    for (int i=0;i<MAXTYPE;i++)
      nBoxNum[i]=bl1->nBoxNum[i] + bl2->nBoxNum[i];
    //��Ԥ�����ٸ��ݾ������͵���
    cEnvelop.x=max(bl1->cEnvelop.x,bl2->cEnvelop.x);
    cEnvelop.y=max(bl1->cEnvelop.y,bl2->cEnvelop.y);
    cEnvelop.z=max(bl1->cEnvelop.z,bl2->cEnvelop.z);
    BLOCK* tmp;
    switch (nType){
      case 1:
        cEnvelop.x=bl1->cEnvelop.x+bl2->cEnvelop.x;
        nUpperX=bl1->nUpperX;
        nUpperY=bl1->nUpperY;
        /*
        tmp=bl2; //ȡ�Ϸ�������
        if (bl1->nUpperX * bl1->nUpperY > bl2->nUpperX * bl2->nUpperY)
          tmp=bl1;
        nUpperX=tmp->nUpperX;
        nUpperY=tmp->nUpperY;*/
        break;
      case 2:
        cEnvelop.y=bl1->cEnvelop.y+bl2->cEnvelop.y;
        nUpperX=bl1->nUpperX;
        nUpperY=bl2->nUpperY;
        /*
        tmp=bl2; //ȡ�Ϸ�������
        if (bl1->nUpperX * bl1->nUpperY > bl2->nUpperX * bl2->nUpperY)
          tmp=bl1;
        nUpperX=tmp->nUpperX;
        nUpperY=tmp->nUpperY;*/
        break;
      case 3:
        cEnvelop.z=bl1->cEnvelop.z+bl2->cEnvelop.z;
        nUpperX=bl2->nUpperX;
        nUpperY=bl2->nUpperY;
        break;
      default:
        exit(-1);
    }
    initHash();
  }

bool BLOCK::isGeneralBlock(){ return nType!=0;}
int BLOCK::display(){
    printf("[BLOCK] %d*%d*%d  [UPPER] %d*%d isSimple:%d nType:%d FillRate:%lf %d %lf\n",
           cEnvelop.x,cEnvelop.y,cEnvelop.z,nUpperX,nUpperY,isSimple,nType,1.0*nBoxVolumn/cEnvelop.getVolumn()*1.0,
           nUpperX * nUpperY , cEnvelop.x*cEnvelop.y * MinAreaRate
           );

    for (int i=1;i<MAXTYPE;i++){
      if (nBoxNum[i])printf("    [%d] numer:%d\n",i,nBoxNum[i]);
    }
}
#define MATCH(w,l,h,env) (env.x%w==0 && env.y%l==0 && env.z%h==0)
int BLOCK::decompose(POINT3 blkPos,vector<CUBOID>& boxType,vector<POINT3>& boxPos,BR_INPUT& input){
    int w,l,h,num,type;
    CUBOID c,box;
    switch(nType){
      case 0: //simple block
         type=isSimple;
        //���Ӵ�С
         box;
         w=input.w[type];
         l=input.l[type];
         h=input.h[type];
         num=nBoxVolumn/(w*h*l);
        //num��������ɵĴ�С 1,2,3  =>2,1,3 =>  3,1,2
        //��1��
         c=cEnvelop;
        if (MATCH(l,w,h,c) || MATCH(l,h,w,c)) swap(w,l);
        if (MATCH(h,w,l,c) || MATCH(h,l,w,c)) swap(w,h);
        if (MATCH(w,h,l,c)) swap(l,h);
        if (!MATCH(w,l,h,c)) exit(1);
        for (int i=0;i<cEnvelop.x/w;i++)
          for (int j=0;j<cEnvelop.y/l;j++)
            for (int k=0;k<cEnvelop.z/h;k++){
              POINT3 pos=blkPos+CUBOID(i*w,j*l,k*h);
              boxType.push_back(CUBOID(w,l,h));
              boxPos.push_back(pos);
            }
        break;
      case 1: //x���
        bl1->decompose(blkPos,boxType,boxPos,input);
        bl2->decompose(blkPos+CUBOID(bl1->cEnvelop.x,0,0),boxType,boxPos,input);
        break;
      case 2: //y���
        bl1->decompose(blkPos,boxType,boxPos,input);
        bl2->decompose(blkPos+CUBOID(0,bl1->cEnvelop.y,0),boxType,boxPos,input);
        break;
      case 3: //z���
        bl1->decompose(blkPos,boxType,boxPos,input);
        bl2->decompose(blkPos+CUBOID(0,0,bl1->cEnvelop.z),boxType,boxPos,input);
        break;
    }
    return 0;
}
//STATE
STATE::STATE(int *nBoxNum,CUBOID* container){
    pbl.clear();
    vol=0;
    for (int i=0;i<MAXTYPE;i++)Bres[i]=nBoxNum[i];
    isComplete=0;
    Rs_stack.push(*container);
    Rs_stack_pos.push(POINT3(0,0,0));
  }

STATE::STATE(STATE *state){                       //����MultiLayerSearchc
    *this=*state;
    for (int i=0;i<MAXTYPE;i++) Bres[i]=state->Bres[i];
  }
//implementation
int MLHS::init(BR_INPUT& input){
  block_hash.clear();
  for (int i=0;i<blockTable.size();i++)delete blockTable[i];
  blockTable.clear();
  container=input.container;
  //���³�ʼ��box table
  //������
  nType=input.nType;
  if (nType<=5) MaxSearchEffort=1<<8;else MaxSearchEffort=1<<4;
  for (int i=1;i<=input.nType;i++){
     nBoxNum[i]=input.nNum[i];
  }
  //���ɲ�ͬ�����block�������6�֣�����3�����������2�����
  for (int i=1;i<=input.nType;i++){
    int w=input.w[i];
    int h=input.h[i];
    int l=input.l[i];

    if (input.ih[i]){                                      //h��Ϊ��
      insertBlock(new BLOCK(l,w,h,i,1));
      insertBlock(new BLOCK(w,l,h,i,1));
    }
    if (input.iw[i]){                                      //w��Ϊ��
      insertBlock(new BLOCK(l,h,w,i,1));
      insertBlock(new BLOCK(h,l,w,i,1));
    }
    if (input.il[i]){                                      //l��Ϊ��
      insertBlock(new BLOCK(w,h,l,i,1));
      insertBlock(new BLOCK(h,w,l,i,1));
    }
  }
  //
  if (state!=NULL)delete state;
  state=new STATE(nBoxNum,&container);
  sol_box.clear();
  sol_boxPos.clear();
}
bool MLHS::isBlockValid(BLOCK* block){
  //(1)C2����
  if (C2_CONSTRAIN){
     //��Ϊz����ʱ�����bl1��������Ƿ��㹻����bl2
     if (block->nType==3)
     if (block->bl1->nUpperX < block->bl2->cEnvelop.x || block->bl1->nUpperY < block->bl2->cEnvelop.y)
       return false;
     //��������ٷֱ�����ΪMinAreaRate
     if (block->nUpperX * block->nUpperY < block->cEnvelop.x*block->cEnvelop.y * MinAreaRate)
      return false;
  }
  //(2)��ά������container
  if (block->cEnvelop.x > container.x || block->cEnvelop.y > container.y || block->cEnvelop.z > container.z) return false;
  //(3)����ٷֱ�����ΪMinFillRate
  if (block->nBoxVolumn < block->cEnvelop.getVolumn()*MinFillRate)
    return false;
  //(4)�������㹻
  for (int i=1;i<=nType;i++)
    if (nBoxNum[i]<block->nBoxNum[i])
    return false;
  //(5)�������
  switch (block->nType){
    case 1:
      if (block->bl1->cEnvelop.y * block->bl1->cEnvelop.z < block->bl2->cEnvelop.y * block->bl2->cEnvelop.z) return false;
      break;
    case 2:
      if (block->bl1->cEnvelop.x * block->bl1->cEnvelop.z < block->bl2->cEnvelop.x * block->bl2->cEnvelop.z) return false;
      break;
    case 3:
      if (block->bl1->cEnvelop.x * block->bl1->cEnvelop.y < block->bl2->cEnvelop.x * block->bl2->cEnvelop.y) return false;
      break;
    default:
      exit(-1);
  }
  //(6)�����Ƿ����ظ�����envelopeһ�£�blockһ�£��������һ�£�
  #ifdef HASH
  if (block_hash.count(block->hash)==0)
      return true;
  return false;
  #endif
  /*
  for (int i=0;i<blockTable.size();i++){
    BLOCK* cur=blockTable[i];
    //��ǰ�鶥�������ͬ/envelope����һ��
    if (cur->nUpperX !=block->nUpperX || cur->nUpperY!=block->nUpperY) continue;
    if (cur->cEnvelop.x !=block->cEnvelop.x) continue;
    if (cur->cEnvelop.y !=block->cEnvelop.y) continue;
    if (cur->cEnvelop.z !=block->cEnvelop.z) continue;
    //nBoxNum�Ƿ���ͬ
    bool same=true;
    for (int j=1;j<=nType;j++)
        if (cur->nBoxNum[j]!=block->nBoxNum[j]){
          same=false;
          break;
        }
    if (same) return false;
  }

  return true;*/
}

int  MLHS::insertBlock(BLOCK *block){
  if (block_hash.count(block->hash)==0 && container.contain(block->cEnvelop)){
    block_hash.insert(block->hash);
    blockTable.push_back(block);
  }else
  delete block;
  return 0;
}
int  MLHS::genSimpleBlock(){  //����Ҫ����isBlockValid��֤
  int sz=blockTable.size();
  for (int boxId=0;boxId<sz;boxId++){
    BLOCK* cur = blockTable[boxId];
    //if (cur->cEnvelop.x <=25 ||cur->cEnvelop.y<=25 || cur->cEnvelop.z<=25)continue;

    int    type= cur->isSimple;
    //ͬһtypeͬһ�������
    for (int nx=1;nx<=nBoxNum[type];nx++)
      for (int ny=1;ny<=nBoxNum[type]/nx;ny++)
        for (int nz=1;nz<=nBoxNum[type]/nx/ny;nz++){
          if (nx*ny*nz == 1)continue;
          //printf("%d %d %d %d %d %d\n",cur->cEnvelop.x * nx,cur->cEnvelop.y * ny,cur->cEnvelop.z * nz,container.x,container.y,container.z);
          if (cur->cEnvelop.x * nx <=container.x && cur->cEnvelop.y*ny<=container.y && cur->cEnvelop.z*nz<=container.z){
            BLOCK *newBlock=new BLOCK(cur->cEnvelop.x * nx,cur->cEnvelop.y*ny,cur->cEnvelop.z*nz,type,nx*ny*nz);

            insertBlock(newBlock);
          }
        }
  }
}
int MLHS::genGeneralBlock(){
  //hash
  int totalBlock=blockTable.size();
  int curLevel_l=0;
  int curLevel_r=blockTable.size()-1;
  for (int level=1;level<=MaxLevel && blockTable.size()<MaxBlocks;level++){
     //����ö�����
     int ts=GetTickCount();
     for (int i=curLevel_l;i<=curLevel_r && blockTable.size()<MaxBlocks; i++)
       for (int j=curLevel_l;j<=curLevel_r && blockTable.size()<MaxBlocks; j++){
          if (level ==1 && i==j) continue;         //general block�˻���simple block��������
          BLOCK* b1=blockTable[i];
          BLOCK* b2=blockTable[j];
          //x���
          if (b1->cEnvelop.y*b1->cEnvelop.z >= b2->cEnvelop.y*b2->cEnvelop.z && isBlockValid(&BLOCK(b1,b2,1))) {
              insertBlock(new BLOCK(b1,b2,1));
          }
          //y���
          if (b1->cEnvelop.x*b1->cEnvelop.z >= b2->cEnvelop.x*b2->cEnvelop.z && isBlockValid(&BLOCK(b1,b2,2))) {
              insertBlock(new BLOCK(b1,b2,2));
          }
          //z���
          if (b1->cEnvelop.x*b1->cEnvelop.y >= b2->cEnvelop.x*b2->cEnvelop.y && isBlockValid(&BLOCK(b1,b2,3))) {
              insertBlock(new BLOCK(b1,b2,3));
          }
       }
       //if (curLevel_r==blockTable.size()-1) {MinFillRate*=MinFillRate;continue;}
       curLevel_l=curLevel_r+1;
       curLevel_r=blockTable.size()-1;
       //printf("Level[%d]%d ms sz=%d\n",level,GetTickCount()-ts,curLevel_r-curLevel_l+1);
  }
}
/********************************************/
bool MLHS::hasEnoughBox(STATE* state,BLOCK* block){     //state���Ƿ�����㹻��box
    for (int i=1;i<=nType;i++)
      if (state->Bres[i] < block->nBoxNum[i])
      return false;
    return true;
}
int  MLHS::generateRSBlockList(STATE* state,CUBOID rs,POINT3 rs_pos,int mxBlocks,vector<BLOCK*>* bll){
   ///
   //���ּ���
   int rs_vol=rs.getVolumn();
   int idx_l=0,idx_r=blockTable.size()-1;
   while (idx_l < idx_r){
     int mid=(idx_l+idx_r)>>1;
     if (blockTable[mid]->cEnvelop.getVolumn() >rs_vol)
       idx_l=mid+1;
     else
       idx_r=mid;
   }
   //����
   bll->clear();
   for (int i=idx_l;i<blockTable.size();i++){
       if (!rs.contain(&(blockTable[i]->cEnvelop))) continue;  //rs̫С
       if (!hasEnoughBox(state,blockTable[i]))continue;//ʣ��box��������
       bll->push_back(blockTable[i]);
       if (bll->size() == mxBlocks+1) break;
   }
   while (bll->size()>mxBlocks){
     int idx=rand()%bll->size();
     swap((*bll)[idx],(*bll)[bll->size()-1]);
     bll->pop_back();
   }
   return 0;
}
int  MLHS::completeState(STATE *s){
   STATE *state=new STATE(s);              //����һ��
   ///vector<BLOCK*> bll;            ///����ջ������ǡ����ˣ���������������������������
   vector<BLOCK*>* bll=new vector<BLOCK*>;
   while (!state->Rs_stack.empty()){
      //ȡջ��space
      CUBOID curRs=state->Rs_stack.top();
      POINT3 curRs_pos=state->Rs_stack_pos.top();
      //����block
      bll->clear();
      generateRSBlockList(state,curRs,curRs_pos,1,bll);
      if (bll->size()==0) {
          state->Rs_stack.pop();
          state->Rs_stack_pos.pop();
          continue;                  //������block����curRs
      }else
          placeBlock(state,(*bll)[0]);
   }
   delete bll;
   state->isComplete=true;
   int vol=state->vol;
   delete state;
   return vol;
}
int  MLHS::placeBlock(STATE *state,BLOCK* block){ //��block����state��ջ����
   #ifdef DEBUG
   if (state->Rs_stack.empty()) exit(-7);
   #endif
   CUBOID space=state->Rs_stack.top();
   POINT3 space_pos=state->Rs_stack_pos.top();
   state->Rs_stack.pop();
   state->Rs_stack_pos.pop();
   #ifdef DEBUG
   if (!space.contain(block->cEnvelop)) {
       printf("Place Block Error:");
       space.display();
       block->cEnvelop.display();
       //state->display();
       exit(-6);
   }
   #endif
   for (int i=1;i<=nType;i++){
       #ifdef DEBUG
       if (state->Bres[i]<block->nBoxNum[i]) exit(-2);
       #endif
       state->Bres[i]-= block->nBoxNum[i];
   }
   state->pbl.push_back(block);
   state->pbl_pos.push_back(space_pos);
   state->vol+=block->nBoxVolumn;
   //ջ������
   generateDaughterSpaces(state,space,space_pos,block,NULL);
   return 0;
}
int  MLHS::removeBlock(STATE *state,BLOCK* block,CUBOID rs,POINT3 rs_pos){//�ָ���������
   for (int i=1;i<=3;i++)  state->Rs_stack.pop(),state->Rs_stack_pos.pop();   //����3���ռ�
   state->pbl.pop_back();
   state->pbl_pos.pop_back();
   state->vol -=block->nBoxVolumn;
   for (int i=1;i<=nType;i++){
     state->Bres[i]+=block->nBoxNum[i];
   }
   state->Rs_stack.push(rs);
   state->Rs_stack_pos.push(rs_pos);
   return 0;
}
/*
int  MLHS::addBlock(STATE *state,BLOCK* block){     //��block����state��,��������rs
   for (int i=1;i<=nType;i++){
       #ifdef DEBUG
       if (state->Bres[i]<block->nBoxNum[i]) exit(-2);
       #endif
       state->Bres[i]-= block->nBoxNum[i];
    }
   state->pbl.push_back(block);
   state->vol+=block->nBoxVolumn;
   return 0;
}*/

int MLHS::generateDaughterSpaces(STATE* state,CUBOID curRs,POINT3 curRs_pos,BLOCK *block,vector<BLOCK*>* blockList=NULL){
  #ifdef DEBUG
  int space_sz=state->Rs_stack.size();
  #endif
  CUBOID blRs=block->cEnvelop;
  int rmx=curRs.x - blRs.x;
  int rmy=curRs.y - blRs.y;
  int rmz=curRs.z - blRs.z;
  //�ж�rmx,rmy,rmz�Ƿ�̫��ʹ�ò�����block���Է���
  bool bx=false;
  bool by=false;
  bool bz=false;
  if (blockList == NULL) blockList=& blockTable;
  for (int i=blockList->size()-1;i>=0;i--){
    if ((*blockList)[i]->cEnvelop.x <=rmx) bx=true;
    if ((*blockList)[i]->cEnvelop.y <=rmy) by=true;
    if ((*blockList)[i]->cEnvelop.z <=rmz) bz=true;

  }
  POINT3 p_x=POINT3(curRs_pos.x+block->cEnvelop.x,curRs_pos.y,curRs_pos.z);
  POINT3 p_y=POINT3(curRs_pos.x,curRs_pos.y+block->cEnvelop.y,curRs_pos.z);
  POINT3 p_z=POINT3(curRs_pos.x,curRs_pos.y,curRs_pos.z+block->cEnvelop.z);
  /*
  printf("daughter:\n");
  block->cEnvelop.display();
  p_x.display();p_y.display();p_z.display();printf("--\n");
  */
  state->Rs_stack_pos.push(p_x);
  state->Rs_stack_pos.push(p_y);
  state->Rs_stack_pos.push(p_z);
  //��һ������C2Լ��ֻ���������(1,2)
  if (C2_CONSTRAIN){
    if (rmx>=rmy ||!by){
        state->Rs_stack.push(CUBOID(rmx,curRs.y,curRs.z));
        state->Rs_stack.push(CUBOID(blRs.x,rmy,curRs.z));
    }else{
        state->Rs_stack.push(CUBOID(rmx,blRs.y,curRs.z));
        state->Rs_stack.push(CUBOID(curRs.x,rmy,curRs.z));
    }
    state->Rs_stack.push(CUBOID(blRs.x,blRs.y,rmz)); //�Ϸ�����С
  }

  else{
  //������������C2Լ��
  //���·���6�֣�������C2Լ����
  if ((rmx >=rmy || !by) && (rmx>=rmz || !bz)){ //rmx => max
     state->Rs_stack.push(CUBOID(rmx,curRs.y,curRs.z));
     if (by &&(rmy >= rmz ||!bz)){  //���һ(����p20)
         state->Rs_stack.push(CUBOID(blRs.x,rmy,curRs.z));
         state->Rs_stack.push(CUBOID(blRs.x,blRs.y,rmz));
     }else{                     //�����
         state->Rs_stack.push(CUBOID(blRs.x,rmy,blRs.z));
         state->Rs_stack.push(CUBOID(blRs.x,curRs.y,rmz));
     }
  }else
  if ((rmy >=rmx || !bx) && (rmy>=rmz || !bz)){//rmy=>max
     if (bx && (rmx>=rmz || !bz)){ //�����

         state->Rs_stack.push(CUBOID(rmx,blRs.y,curRs.z));
         state->Rs_stack.push(CUBOID(curRs.x,rmy,curRs.z));
         state->Rs_stack.push(CUBOID(blRs.x,blRs.y,rmz));
     }else{                //�����
         state->Rs_stack.push(CUBOID(rmx,blRs.y,blRs.z));
         state->Rs_stack.push(CUBOID(curRs.x,rmy,curRs.z));
         state->Rs_stack.push(CUBOID(curRs.x,blRs.y,rmz));

     }
  }
  else{
     //printf("in rmx=%d rmy=%d rmz=%d  ",rmx,rmy,rmz);curRs.display();block->cEnvelop.display();
     if (bx && (rmx >= rmy || !by)){ //�����
         state->Rs_stack.push(CUBOID(rmx,curRs.y,blRs.z));
         state->Rs_stack.push(CUBOID(blRs.x,rmy,blRs.z));
     }else{                         //�����
         state->Rs_stack.push(CUBOID(rmx,blRs.y,blRs.z));//printf("debug:");CUBOID(rmx,blRs.y,blRs.z).display();
         state->Rs_stack.push(CUBOID(curRs.x,rmy,blRs.z));//printf("debug:");CUBOID(curRs.x,rmy,blRs.z).display();
     }
     state->Rs_stack.push(CUBOID(curRs.x,curRs.y,rmz));//printf("debug:");CUBOID(curRs.x,curRs.y,rmz).display();
     }//��bx,by,bz��̫С�Ͳ�Ҫ����
  }
  #ifdef DEBUG
  if (state->Rs_stack.size()-space_sz!=3)exit(-8);
  CUBOID c1=state->Rs_stack.top();state->Rs_stack.pop();
  CUBOID c2=state->Rs_stack.top();state->Rs_stack.pop();
  CUBOID c3=state->Rs_stack.top();state->Rs_stack.pop();
  if (c1.getVolumn()+c2.getVolumn()+c3.getVolumn()+block->cEnvelop.getVolumn()!=curRs.getVolumn())exit(-9);
  state->Rs_stack.push(c3);
  state->Rs_stack.push(c2);
  state->Rs_stack.push(c1);
  #endif
  return 0;
}

int  MLHS::dfs(STATE* state,int depth,int branch){
//return value: state�������������С��
  if (depth !=0){
     //residual spaceΪ��
     if (state->Rs_stack.size()==0){
       return state->vol;
     }
     //ȡջ���ռ�
     CUBOID rs=state->Rs_stack.top();
     POINT3 rs_pos=state->Rs_stack_pos.top();

     vector<BLOCK*> *blockList=new vector<BLOCK*>;
     blockList->clear();
     generateRSBlockList(state,rs,rs_pos,LayerExt,blockList);

     //����������rs��block
     if (blockList->size()==0){
        state->Rs_stack.pop();
        state->Rs_stack_pos.pop();
        int v=dfs(state,depth,branch);
        state->Rs_stack.push(rs); //�ָ�
        state->Rs_stack_pos.push(rs_pos);
        delete blockList;
        return v;
     }else{
         int mxVol=state->vol;
         for (int i=0;i<min(branch,(int)blockList->size());i++){   //ѡ��branch��������
             //����block
             placeBlock(state,(*blockList)[i]);
             int k=dfs(state,depth-1,branch);
             //�ָ�
             removeBlock(state,(*blockList)[i],rs,rs_pos);
             mxVol=max(mxVol,k); //ά������ֵ
         }
         delete blockList;
         return mxVol;
     }
  }else{
     return completeState(state);
  }
}
//�ҵ����ŵ���һ�顣ִ�к󲻸ı�state
BLOCK* MLHS::findNextBlock(STATE* s,int search_effort){ //�����ڷ���null
  //������rs���˳�
  if (s->Rs_stack.empty())return NULL;
  STATE *state=new STATE(s);
  //ȫ������
  BLOCK *bestBlock=NULL;
  int    bestVolum=state->vol; //���Բ��Һ����ҵ�����õġ�
  //����heap
  HEAP *curHeap,*nextHeap;
  curHeap=new HEAP(MaxHeap);
  nextHeap=new HEAP(MaxHeap);
  //Layer 0
  int pbl_sz=state->pbl.size();
  curHeap->push(state,0);      //��0��ֻ��1������ֵ����ν(���⣬�����������heap,heap.clear��delete�ڲ�state���������ٴ�delete)

  vector<BLOCK*>* bll=new vector<BLOCK*>;

  for (int layer=1;layer<=MaxLayer;layer++){
    for (int i=0;i<curHeap->size();i++){ //ö�ٵ�ǰlayer
      STATE* curState=curHeap->get(i);
      //��������ڵ�
      //ջ���ռ�
      bll->clear();
      CUBOID space;
      POINT3 space_pos;
      //�ҵ�һ�����Բ�������ڵ��space
      bool find=false;
      while (!find){
         if (curState->Rs_stack.empty())break;
         space=curState->Rs_stack.top();
         space_pos=curState->Rs_stack_pos.top();
         generateRSBlockList(curState,space,space_pos,MaxHeap,bll);
         if (bll->size()!=0)find=true;
         else{
           if (layer==1) {s->Rs_stack.pop();s->Rs_stack_pos.pop();}
           curState->Rs_stack.pop();
           curState->Rs_stack_pos.pop();
         }
      }
      if (!find)continue;//��ǰstateû�к����ڵ�
      for (int i=0;i<bll->size();i++){
          BLOCK* nxtBlk=(*bll)[i];
          STATE* nxtState=new STATE(curState);
          placeBlock(nxtState,nxtBlk);
          //��ǰ��������curState����nxtBlk���γ�nxtState�����nxtState���в�ͬ��ȵ�������
          //����nxtState���в�ͬ��������������ֵ
          int nxtBestVol=nxtState->vol;//����������ͬ��ȣ��Ի�ȡ��ǰ����(nxtBlk)���Դﵽ�����
          for (int d=1;d<=MaxDepth;d++){
              int branch=GETBRANCH(d,search_effort);
              if (branch==1) branch=2;
              int curVol=dfs(nxtState,d,branch);
              //����global best
              if ( nxtBestVol <curVol)
                nxtBestVol=curVol;
          }
          //����curState+nxtBlk=CurState����õ����ΪnxtBestVol��������һ��layer
          nextHeap->push(nxtState,nxtBestVol);
          //��������state;�У�������state complete����ÿ�����ɶ�����ȫ������
          //printf("[nxtState]vol:%d %d\n",nxtBestVol,nextHeap->size());
          if (bestVolum<nxtBestVol){
            bestVolum=nxtBestVol;
            bestBlock=nxtState->pbl[pbl_sz];
          }
      }
    }

    if (nextHeap->size()==0) break;          //�Ѳ����¸���
    swap(curHeap,nextHeap);
    nextHeap->clear();
  }
  curHeap->clear();
  nextHeap->clear();
  delete curHeap;
  delete nextHeap;
  return bestBlock;
}
int MLHS::accelerate(){
  int mx_x=0,mx_y=0,mx_z=0,mx_v=0;
  //���»�ȡ����rs�У����������x���С��y���С��z���С
  vector<CUBOID>* rs=new vector<CUBOID>;
  while (!state->Rs_stack.empty()){
    CUBOID curRs=state->Rs_stack.top();
    state->Rs_stack.pop();
    mx_x=max(mx_x,curRs.x);
    mx_y=max(mx_y,curRs.y);
    mx_z=max(mx_z,curRs.z);
    mx_v=max(mx_v,curRs.getVolumn());
    rs->push_back(curRs);
  }
  //�ָ�rs stack
  //sort(rs->begin(),rs->end(),cmp_rs);//�Ӵ�С����rs
  for (int i=rs->size()-1;i>=0;i--){
    state->Rs_stack.push((*rs)[i]);
  }
  delete rs;
  //blocktable��С
  int curIdx=0;
  for (int i=0;i<blockTable.size();i++){
    CUBOID cb=blockTable[i]->cEnvelop;
    //̫���blockɾȥ
    if (cb.x>mx_x && cb.y>mx_y && cb.z>mx_z &&cb.getVolumn()>mx_v) continue;
    //box�����ɾȥ
    bool notEnoughBox=false;
    for (int boxId=1;boxId<=nType;boxId++){
      if (state->Bres[boxId] < blockTable[i]->nBoxNum[boxId]) {notEnoughBox=true;break;}
    }
    if (notEnoughBox) continue;
    blockTable[curIdx++]=blockTable[i];
    //else
    //  delete blockTable[i]; //ע��pbl�б����block���ܱ�delete��
  }
  while (blockTable.size()>curIdx) blockTable.pop_back();
  return 0;
}
double MLHS::Work(BR_INPUT &input,bool bGeneral=true){
  //(1)��ʼ��
  init(input);
  //(2)����blocks�������������
  genSimpleBlock();
  bGeneral && genGeneralBlock();
  sort(blockTable.begin(),blockTable.end(),cmp_block_table);
  int search_effort=1;
  while (!state->isComplete){
    BLOCK *nextBlock=findNextBlock(state,search_effort);
    if (nextBlock == NULL){state->isComplete=true;break;}
    placeBlock(state,nextBlock);
    nextBlock->decompose(state->pbl_pos[state->pbl_pos.size()-1] ,sol_box,sol_boxPos,input);
    //blockTable[0]->display();
    if (search_effort<MaxSearchEffort)search_effort<<=1;
    #ifdef DEBUG
    printf("Work NextBlock:");nextBlock->display();
    #endif
    accelerate();
  }

  return 1.0*state->vol/container.getVolumn();
}

/**************************/
//��������
vector<BR_INPUT> data;
void read(char *filename){
  if (!freopen(filename,"r",stdin)){
    exit(-1);
  }
  int ks=0;
  scanf("%d",&ks);
  data.resize(ks);
  for (int i=0;i<ks;i++){
    int t1,t2;
    scanf("%d%d",&t1,&t2);  //t1��case index��t2�������������
    data[i].read();
  }
}
MLHS mlhs;
int main(int argc,char** argv){
    memset(occupy,0,sizeof(occupy));
    occupy[0][0][2]=1;
    memset(occupy,0,sizeof(occupy));
    printf("%d\n",occupy[0][0][2]);
  if (argc<2){
      printf("ʹ�÷�������:\n3d-container.exe  data/thpack1.txt\n");
      printf("���Ҫ���box���ò��ԣ���main������data[i].output_strategy(mlhs.sol_box,mlhs.sol_boxPos)��ע��ȡ������");
      return 0;
  }
  read(argv[1]);
  double avgUsage=0;
  for (int i=0;i<10;i++){
    int ts=time(NULL);
    double usageRate=mlhs.Work(data[i],GenGeneralBlock);
    double usageRate2=data[i].solution_verify(mlhs.sol_box,mlhs.sol_boxPos);

    printf("[case:%d] ִ�н��:%.6lf\n",i,usageRate);
    printf("         ��֤���:%.6lf\n",usageRate2);       //���÷����洢��mlhs.sol_box()��mlhs.sol_bosPos()��
    //data[i].output_strategy(mlhs.sol_box,mlhs.sol_boxPos);//���box���ò���
    avgUsage+=usageRate;
    int te=time(NULL);
    printf("         ����ʱ��:%d s\n",te-ts);
  }
  printf("ƽ��ʹ����:%.6lf\n",avgUsage/10);
  return 0;
}
