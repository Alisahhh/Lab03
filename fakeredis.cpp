#include"type.h"
#include"baseFunction.h"

int main(){
  fakeServer *server=buildServer();
  if(server==nullptr){
    cout<<"Can't build server";
    return 0;
  }
  int cnt=0;
  //cout<<"OK"<<endl;
  //cout<<++cnt<<endl;
  while(1)
  {
    //cout<<++cnt<<endl;
    reciveAndSend (server);
  }
}
