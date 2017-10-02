#include<bits/stdc++.h>
using namespace std;
struct DATA{
    int x;
    DATA(int t){x=t;}
    DATA(){}
/*产生数学后继*/
    DATA operator()(){
        return DATA(x+1);
    }
};
/* 下列比较函数只需要更改<即可 */
bool operator <(const DATA &a,const DATA &b){
    return a.x<b.x;
}
bool operator == (const DATA &a,const DATA &b){
    return !(a<b||b<a);
}
struct NODE{
    int size,HASH,left,right;
    int RANDOM;
    DATA data;
    NODE(){
        left=right=size=0;
    }
};
int top;
NODE tree[100000];
struct TREAP{
    int root;
    TREAP(){
        root=0;
        srand(time(NULL));
    }
    void adapt(int x){
		tree[x].size=tree[tree[x].left].size+tree[tree[x].right].size+1;
	}
    void left_rot(int fa,int x){
		int y=tree[x].right;
		tree[x].right=tree[y].left;
		tree[y].left=x;
		tree[y].size=tree[x].size;
		if (fa){
            if (tree[fa].left==x) tree[fa].left=y;
            else tree[fa].right=y;
		}
		adapt(x);
		root=root==x?y:root;
	}
	void right_rot(int fa,int x){
		int y=tree[x].left;
		tree[x].left=tree[y].right;
		tree[y].right=x;
		tree[y].size=tree[x].size;
        if (fa){
            if (tree[fa].left==x) tree[fa].left=y;
            else tree[fa].right=y;
		}
		adapt(x);
		root=root==x?y:root;
	}
    void insert(int fa,int x,DATA data){
		if (x==0){
			x=++top;
			if (!root) root=top;
			tree[x].size=1;
			tree[x].left=tree[x].right=0;
			tree[x].data=data;
			tree[x].HASH=top;
			tree[x].RANDOM=rand();
		}
		else{
			tree[x].size++;
			if (data<tree[x].data||(data==tree[x].data)&&(tree[x].HASH>(top+1))) {
                insert(x,tree[x].left,data);
                if (!tree[x].left) tree[x].left=top;
                if (tree[x].RANDOM>tree[tree[x].left].RANDOM) right_rot(fa,x);
            }
			else {
                insert(x,tree[x].right,data);
                if (!tree[x].right) tree[x].right=top;
                if (tree[x].RANDOM>tree[tree[x].right].RANDOM) left_rot(fa,x);
            }
		}
	}
    int remove(int fa,int x,DATA data){
		if (!x) return 0;
		if (tree[x].data<data){
			int kk=remove(x,tree[x].right,data);
			if (kk) tree[x].size--;
			return kk;
        }
		else
		if (data<tree[x].data){
			int kk=remove(x,tree[x].left,data);
			if (kk) tree[x].size--;
			return kk;
        }
		else{
			if (tree[x].left!=0&&tree[x].right==0){
				tree[x]=tree[tree[x].left];
				return x;
			}
			else
			if (tree[x].right!=0&&tree[x].left==0){
				tree[x]=tree[tree[x].right];
				return x;
			}
			else
			if (tree[x].left==0&&tree[x].right==0){
				if (root==x) root=0;
				if (fa)
					if (tree[fa].left==x) tree[fa].left=0;
					else tree[fa].right=0;
                return x;
			}
			else{
				int ret=tree[x].right,faa=x;
				tree[x].size--;
				while (tree[ret].left){
                    tree[ret].size--;
                    faa=ret;
                    ret=tree[ret].left;
				}

				if (faa==x) tree[x].right=tree[ret].right;
				else tree[faa].left=tree[ret].right;

                tree[x].data=tree[ret].data;
                return x;
			}
		}
	}
	void Insert(DATA data){
        insert(0,root,data);
	}
	int Remove(DATA data){
        return remove(root,root,data);
	}
	//查询有多少个数比x小
	int rank(int &x,DATA data){
		if (x==0) return 0;
		if (tree[x].data<data) return tree[tree[x].left].size+rank(tree[x].right,data)+1;
		else return rank(tree[x].left,data);
	}
	int Rank(DATA data){return rank(root,data);}

    DATA select(int &x,int k){
		int r=tree[tree[x].left].size+1;
		if (k<r) return select(tree[x].left,k);
		else
		if (k>r) return select(tree[x].right,k-r);
		else
		return tree[x].data;
	}
	DATA Select(int k){
        return select(root,k);
	}
    int Find(DATA data){
		if (!root) return 0;
		else {
			int x=root;
			while (!(tree[x].data==data)&&x){
				if (tree[x].data<data) x=tree[x].right;
				else x=tree[x].left;
			}
			if (!x) return 0;
			else if (tree[x].data==data) return x;
		}
	}
	int Count(DATA data){
		bool exi=Find(data);
		if (exi){
			int aaa=Rank(data);
			int bbb=Rank(data());
			return bbb-aaa;
		}
		else return 0;
	}
	void show(int x){
        cout<<tree[x].data.x<<endl;
        if (tree[x].left) show(tree[x].left);
        if (tree[x].right) show(tree[x].right);
	}
} a;
int main(){
    for (int i = 0;i<10;i++)
        a.Insert(DATA(i));
    for (int i = 0;i<5;i++)
        a.Remove(i);

    a.show(a.root);
    return 0;
}
