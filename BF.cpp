//基础版本
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <bitset>
#include <time.h>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "BOBHash32.h"
#define TOP_MAX 4

using namespace std;
double TOP_OUT=0; 
class Top4{
public:
	int id[TOP_MAX];
	int hi[TOP_MAX];
	int lo[TOP_MAX];
	//bool empty[TOP_MAX];
	int left;
	
	void clear(){
		for(int i=0;i<TOP_MAX;i++){
			id[i]=0;
			hi[i]=0;
			lo[i]=0;
			//empty[i]=1;
		}
		left=0;
	}
	void kickOut(int i,int x){
		/*if(x>0){
			for(int i=0;i<TOP_MAX;i++){
				cout<<id[i]<<","<<hi[i]<<","<<lo[i]<<endl;
			}
			cout<<left<<"____"<<endl;
		}*/
		int temp=hi[i]; 
		hi[i]=0;
		left+=temp;
		for(int j=0;j<i;j++){
			lo[j]-=temp;
			if(lo[j]<=int(TOP_OUT*hi[j])&&hi[j]>0) kickOut(j,0);
		}
		for(int j=i+1;j<TOP_MAX;j++){
			lo[j]-=temp;
			if(lo[j]<=int(TOP_OUT*hi[j])&&hi[j]>0) kickOut(j,0);
		}
		
	} 
	void insert(int x){
		bool exist =0;
		for(int i=0;i<TOP_MAX;i++){
			if(id[i]==x){
				hi[i]++;
				lo[i]++;
				exist=1;
				break;
			}
		}
		for(int i=0;i<TOP_MAX;i++){
			if(hi[i]<=0){
				id[i]=x;
				hi[i]=lo[i]=1;
				exist=1;
				break;
			}
		}
		if(!exist){
			for(int i=0;i<TOP_MAX;i++){
				lo[i]--;
				if(lo[i]<=int(TOP_OUT*hi[i])&&hi[i]>0){
					kickOut(i,1);
				}
			}
			left++;
		}
			
		
	}
	int query(int x){
		for(int i=0;i<TOP_MAX;i++){
			if(id[i]==x) return  (hi[i]+lo[i]+left)/2;
		} 
		int result=0;
		for(int i=0;i<TOP_MAX;i++){
			result+=hi[i]-lo[i];
		}
		result/=TOP_MAX;
		return (left+result)/2;
	}
};

class slideBF{
public:    
	
         
    static const int 
        MAXID = 1e7,
        MAX_HASH_NUM = 50;
    int
        window,//滑动窗口大小
        memory,//内存使用
        //tim_size,//时间戳大小
        hash_num,//哈希个数
        maxid,//最大id
        group_size,
        maxgid;
    int maxidTop4,maxidCm;
    double tim_rat;//观察时间与窗口比例
    BOBHash32 hash[MAX_HASH_NUM];
    BOBHash32 hashg;
    int cm_hi[MAXID];
    int cm_lo[MAXID];
    int cu_hi[MAXID];
    int pr_lo[MAXID];
    int cm_id[MAXID];
    
    int mycu_hi[MAXID];
    int mycu_lo[MAXID];
    int mycu_id[MAXID];
    int cm[MAXID];
    int cu[MAXID];
    Top4 top4[MAXID];
    int T;
    void init(int _memory,
              int _hash_num){
        memory = _memory,
        hash_num = _hash_num,
        maxid = memory;
        maxidTop4=maxid*3/13;
        maxidCm=maxid*3;
        
        for(int i=0;i<hash_num;i++){
            hash[i].initialize((i+1)%MAX_PRIME32);
        }
        for(int i=0;i<maxid;i++){
        	cm_hi[i]=cu_hi[i]=cm_lo[i]=cm_id[i]=pr_lo[i]=mycu_hi[i]=mycu_lo[i]=mycu_id[i]=0; 
        	top4[i].clear();
		}
		for(int i=0;i<maxidCm;i++) cm[i]=cu[i]=0;
            
    }
    void insert(int x){
		
		
        int mycu=querymycu(x);
        int minn=2*1e9;
        for(int i=0;i<hash_num;i++){
            int pos = hash[i].run((char*)&x, sizeof(int))%maxid;
            minn=min(minn,cu_hi[pos]);
        }
        
        for(int i=0;i<hash_num;i++){
            int pos = hash[i].run((char*)&x, sizeof(int))%maxid;
            cm_hi[pos]++;
            
            if(cu_hi[pos]==minn)
                cu_hi[pos]++;
            
            if(cm_id[pos]==x){
                cm_lo[pos]++,
                pr_lo[pos]++;
            }
            else
            {
                if(cm_lo[pos]==0){
                    cm_lo[pos]++;
                    cm_id[pos]=x;
                    pr_lo[pos]=0;
                }
                else
                    cm_lo[pos]--;
            }

            if((mycu_id[pos]==x?(mycu_hi[pos]+mycu_lo[pos])/2:(mycu_hi[pos]-mycu_lo[pos])/2)!=mycu)continue;

            mycu_hi[pos]++;
            
            if(mycu_id[pos]==x){
                mycu_lo[pos]++;
            }
            else
            {
                if(mycu_lo[pos]==0){
                    mycu_lo[pos]++;
                    mycu_id[pos]=x;
                }
                else
                    mycu_lo[pos]--;
            }
        }
    }
    void insertTop4(int x){
    	top4[hash[0].run((char*)&x, sizeof(int))%maxidTop4].insert(x);
	} 
	void insertCm(int x){
        int minn=2*1e9;
        for(int i=0;i<hash_num;i++){
            int pos = hash[i].run((char*)&x, sizeof(int))%maxidCm;
            minn=min(minn,cu[pos]);
        }
        
        for(int i=0;i<hash_num;i++){
            int pos = hash[i].run((char*)&x, sizeof(int))%maxidCm;
            cm[pos]++;
            
            if(cu[pos]==minn)
                cu[pos]++;
        }
	}
	double queryCm(int x){
        int cuminn=2*1e9;
        for(int i=0;i<hash_num;i++){
            int pos = hash[i].run((char*)&x, sizeof(int))%maxidCm;
            cuminn=min(cuminn,cm[pos]);
            // myminn=min(myminn,(cm_hi[pos]+(cm_id[pos]==x?cm_lo[pos]:0))/2);
        }
        return cuminn;
    }
    double queryCu(int x){
        int cuminn=2*1e9;
        for(int i=0;i<hash_num;i++){
            int pos = hash[i].run((char*)&x, sizeof(int))%maxidCm;
            cuminn=min(cuminn,cu[pos]);
            // myminn=min(myminn,(cm_hi[pos]+(cm_id[pos]==x?cm_lo[pos]:0))/2);
        }
        return cuminn;
    }
    double querycu(int x){
        int cuminn=2*1e9;
        for(int i=0;i<hash_num;i++){
            int pos = hash[i].run((char*)&x, sizeof(int))%maxid;
            cuminn=min(cuminn,cu_hi[pos]);
            // myminn=min(myminn,(cm_hi[pos]+(cm_id[pos]==x?cm_lo[pos]:0))/2);
        }
        return cuminn;
    }
    
    double querycm(int x){
        int cmminn=2*1e9;
        for(int i=0;i<hash_num;i++){
            int pos = hash[i].run((char*)&x, sizeof(int))%maxid;
            cmminn=min(cmminn,cm_hi[pos]);
            // myminn=min(myminn,(cm_hi[pos]+(cm_id[pos]==x?cm_lo[pos]:0))/2);
        }
        return cmminn;
    }
    double querymy(int x){
        int myminn=2*1e9;
        for(int i=0;i<hash_num;i++){
            int pos = hash[i].run((char*)&x, sizeof(int))%maxid;
            myminn=min(myminn,(cm_hi[pos]+cm_lo[pos])/2);
            if(cm_id[pos]!=x)myminn=min(myminn,(cm_hi[pos]-cm_lo[pos])/2);
        }
        return myminn;
    }
    double querymycu(int x){
        int myminn=2*1e9;
        for(int i=0;i<hash_num;i++){
            int pos = hash[i].run((char*)&x, sizeof(int))%maxid;
            myminn=min(myminn,(mycu_hi[pos]+mycu_lo[pos])/2);
            if(mycu_id[pos]!=x)myminn=min(myminn,(mycu_hi[pos]-mycu_lo[pos])/2);
        }
        return myminn;
    }
    double querylo(int x){
        int lo=0;;
        for(int i=0;i<hash_num;i++){
            int pos = hash[i].run((char*)&x, sizeof(int))%maxid;
            if(x==cm_id[pos])lo=max(lo,cm_lo[pos]);
        }
        return lo;
    }
    double queryhit(int x){
        int hit=0;
        for(int i=0;i<hash_num;i++){
            int pos = hash[i].run((char*)&x, sizeof(int))%maxid;
            if(cm_id[pos]==x)
                hit=1;
        }
        return hit;
    }
    double queryTop4(int x){
    	return top4[hash[0].run((char*)&x, sizeof(int))%maxidTop4].query(x);
	}
}sbf;



const int MAXNUM=1.1*1e7;
//insnum不超过1400w

unordered_map<int,int>inmap;
unordered_map<int,int>inmap2;
unordered_map<int,int>inmap3;
vector<int> flow;
//maxnum小于3000w
void load_data1_CAIDA(){
    BOBHash32 hash_id;
    hash_id.initialize(0%MAX_PRIME32);
    
    ifstream input( "formatted00.dat", ios::in | ios::binary );
    char buf[2000]={0};
    for(int i=1;i<=MAXNUM;i++) {
        if(!input.read(buf, 16)){
            printf("ERROR\n");
            break;
        }
        flow.push_back(hash_id.run(buf+8, 16));
    }
}
const int WS=100;
const int insnum=1e7;
double ans[insnum+5];

void test(int bucketNum){
    // random_shuffle(flow.begin(),flow.end());
    double ss=0;
    for(int i=1;i<=insnum;i++) {
            inmap[flow[i]]=0;
            inmap2[flow[i]]=0;
            inmap3[flow[i]]=0;
    }
    sbf.init(bucketNum,3);
    for(int i=1;i<=insnum;i++) {
            inmap[flow[i]]++;
            inmap2[flow[i]]++;
            inmap3[flow[i]]++;
            sbf.insert(flow[i]);
            sbf.insertTop4(flow[i]);
            sbf.insertCm(flow[i]);
        }
        
    
    double flownum=0;
    double higher=0;
    double noerro=0;
    double noerro_cu=0;
    double lower=0;
    double hit=0;
    double aaecu=0,aaecm=0,aaemy=0,aaemycu=0,aaeTop4=0;
    double arecu=0,arecm=0,aremy=0,aremycu=0,areTop4=0; 
    double are=0;
    int whi=0,weq=0,wlo=0;
    const int bound=4000;
    //const int bound=1500;
    int topp=0;
    for(int i=1;i<=insnum;i++) {
        if(inmap3[flow[i]]<bound&&inmap3[flow[i]]>0){
            flownum++;
            double cu=sbf.queryCu(flow[i]),
                    cm=sbf.queryCm(flow[i]),
                    my=sbf.querymy(flow[i]),
                    mycu=sbf.querymycu(flow[i]),
                    cmTop4=sbf.queryTop4(flow[i]),
                    re=inmap3[flow[i]];
            if(re>my)lower++;
            if(re==my)noerro++;
            if(re<my)higher++;
            aaecu+=cu-re;
            aaecm+=cm-re;
            aaemy+=fabs(my-re);if(my-re<0){printf("Error1");return;}
            aaemycu+=fabs(mycu-re);if(mycu-re<0){printf("Error2");return;}
            aaeTop4+=fabs(cmTop4-re);
            arecu+=(cu-re)/re;
            arecm+=(cm-re)/re;
            aremy+=fabs(my-re)/re;if(my-re<0){printf("Error1");return;}
            aremycu+=fabs(mycu-re)/re;if(mycu-re<0){printf("Error2");return;}
            areTop4+=fabs(cmTop4-re)/re;
            
            hit+=sbf.queryhit(flow[i]);
            inmap3[flow[i]]=0;
        }
        
    }
    double precision=0;
    vector<pair<int,int> >v;
    for(int i=0;i<sbf.maxid;i++){
        int s=(sbf.mycu_hi[i]+sbf.mycu_lo[i])/2;
        if(s>sbf.cu_hi[i])
            whi++;
        if(s==sbf.cu_hi[i])
            weq++;
        if(s<sbf.cu_hi[i])
            wlo++;
        // if(inmap[sbf.cm_id[i]]>1&&sbf.querymy(sbf.cm_id[i])>bound){
        //     topp++,
        //     inmap[sbf.cm_id[i]]=0;
        //     v.push_back(make_pair(-(int)sbf.querymy(sbf.cm_id[i]),(int)sbf.cm_id[i]));
        // }
        
        if(inmap[sbf.mycu_id[i]]>1&&sbf.querymycu(sbf.mycu_id[i])>bound){
            topp++,
            inmap[sbf.mycu_id[i]]=0;
            v.push_back(make_pair(-(int)sbf.querymycu(sbf.mycu_id[i]),(int)sbf.mycu_id[i]));
        }
    }
    sort(v.begin(),v.end());
    //printf("real   est(sort)  id\n");
    for(int i=0;i<flownum&&i<100;i++){
        if(inmap2[v[i].second]>bound)
            precision++;
        //cout<<inmap2[v[i].second]<<(inmap2[v[i].second]==-v[i].first?"=":" ")<<-v[i].first<<" "<<v[i].second<<endl;
    }
    //printf("topp:%d precision:%.3lf\n",topp,precision/flownum);
    //printf("whi:%d\tweq:%d\twlo:%d\n",whi,weq,wlo);

    cerr << "flownum=" << flownum << endl;
    aaecu/=flownum;
    aaecm/=flownum;
    aaemy/=flownum;
    aaemycu/=flownum;
    aaeTop4/=flownum;
    arecu/=flownum;
    arecm/=flownum;
    aremy/=flownum;
    aremycu/=flownum;
    areTop4/=flownum;
    //printf("flow_num:%.lf\n higher:%.lf\tnoerror:%.lf\tlower:%.lf\n",flownum,higher,noerro,lower);
    printf("AAE_cu:%.3lf\t AAE_cm:%.3lf\t AAE_my:%.3lf\t AAE_mycu:%.3lf\t AAE_top4:%.3lf\n",aaecu,aaecm,aaemy,aaemycu,aaeTop4);
    printf("ARE_cu:%.3lf\t ARE_cm:%.3lf\t ARE_my:%.3lf\t ARE_mycu:%.3lf\t ARE_top4:%.3lf\n",arecu,arecm,aremy,aremycu,areTop4);
}
double  testTop4(int bucketNum,double top){
	TOP_OUT=top;
    for(int i=1;i<=insnum;i++) {
            inmap[flow[i]]=0;
            inmap2[flow[i]]=0;
            inmap3[flow[i]]=0;
    }
    sbf.init(bucketNum,3);
    for(int i=1;i<=insnum;i++) {
            inmap[flow[i]]++;
            inmap2[flow[i]]++;
            inmap3[flow[i]]++;
            sbf.insertTop4(flow[i]);
        }
        
    double aaeTop4=0,areTop4=0,flownum=0;  
    const int bound=0;
    for(int i=1;i<=insnum;i++) {
        if(inmap3[flow[i]]>bound){
            //flownum++;
            double cmTop4=sbf.queryTop4(flow[i]),
                re=inmap3[flow[i]];
            aaeTop4+=fabs(cmTop4-re);
            areTop4+=fabs(cmTop4-re)/re;
            flownum++;
            inmap3[flow[i]]=0;
        }
        
    }
    
    aaeTop4/=flownum;
    areTop4/=flownum;
    return aaeTop4;
}
void paramTop4(int bucketNum, double min, double max, double error){
	double tryLow,tryHigh,g; 
	g=(sqrt(5)-1.0)/2;
	tryLow=min+(1-g)*(max-min);
	tryHigh=min+g*(max-min);
	while(fabs(max-min)>error){
		//cout<<"MAXMIN"<<max<<" "<<min<<endl; 
		if(testTop4(bucketNum,tryLow)>testTop4(bucketNum,tryHigh)){
			min=tryLow;
			tryLow=tryHigh;
			tryHigh=min+g*(max-min);
		}else{
			max=tryHigh;
			tryHigh=tryLow;
			tryLow=min+(1-g)*(max-min);
		}
	}
	TOP_OUT= (max+min)/2;

} 
int main(){

    srand(2020);
    load_data1_CAIDA();
    TOP_OUT=0;
    cout<<"FlowNum:"<<392594<<endl;
    int bucketNum=8000;
    for(int i=1;i<11;i+=1){
    	
    	//paramTop4(39259*i,-10,0,0.1);
    	//cout<<"bucketNum:"<<39259*i<<"param:"<<TOP_OUT<<endl;
    	cout<<"bucketNum:"<<(bucketNum<<i)<<endl;
    	test(bucketNum<<i);
    	cout<<endl;
		//cout<<"AAE_top4 param=0:"<<testTop4(39259*i,0)<<endl;
	}
    
    
    return 0;
}
