#include <SDL2/SDL.h>
#include <cstdint>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define G goto
#define Q case
#define B break;
#define K return
#define Y1(_)V(_)p=e0[(o-_)/8];
#define Y2(_)Y4(_)X(_+48)m=o-_;
#define Y0(_)Y3(_)Y3(_+32)m=o-_;
#define Y3(_)Q _:Q _+8:Q _+16:Q _+24:
#define V(_)Y3(_)Q _+32:Q _+40:Q _+56:
#define W(_)Q _:Q _+16:Q _+32:Q _+48:m=o-_;
#define Y4(_)V(_)V(_+1)V(_+2)V(_+3)V(_+4)V(_+5)V(_+7)m=o-_;
#define D(_)Q _+6:Q _+70:X(_)x=(o==_+6)?r():(o==_+70)?y():*p;
#define X(_)Q _:Q _+1:Q _+2:Q _+3:Q _+4:Q _+5:Q _+7:p=e0[o&7];
#define Y5(_,y)Y3(_)Q y:c=(o==y)||(F&FM[(o-_)/8])==FE[(o-_)/8];

using z=uint8_t;using Z=uint16_t;using I=int;
z o,m,u,x,c,n,*r0,*r1,i[512],v[8192],r4[16384],*r2,*r3,e[]={19,0,216,0,77,1,176,1,254,255},&F=e[6],&A=e[7],
*e0[]={e+1,e,e+3,e+2,e+5,e+4,&F,&A},&IF=i[271],&O=i[320],&L=i[324],IM,h,*p;
z const*J;
Z P=256,U,*E=(Z*)e,&H=E[2],&S=E[4],&DI=(Z&)i[259],d=32,*M,*E0[]={E,E+1,&H,&S},*E1[]={E,E+1,&H,&H};
uint32_t fb[23040],pal[]={0xffffffff,0xffaaaaaa,0xff555555,0xff000000},C_,C;
I s,t,HA[]={0,0,1,-1},FM[]={128,128,16,16},FE[]={0,128,0,16};

void T(){C+=4;}z me(Z a, z x, I w){T();switch(a>>12){Q 2:Q 3:if(w)r1=r0+((x?x&63:1)<<14);Q 0:Q 1:K r0[a];Q 4:Q 5:
if(w&&x<=3)r3=r2+(x<<13);Q 6:Q 7: K r1[a&16383];Q 8:Q 9:a&=8191;if(w)v[a]=x;K v[a];Q 10:Q 11:a&=8191;if(w)
r3[a]=x;K r3[a];Q 15:if(a>=65024){if(w){if(a==65350)for(I y=160;--y>=0;)i[y]=me(x*256+y,0,0);i[a&511]=x;}if(a==
65280){if(!(i[256]&16))K~(16+J[81]*8+J[82]*4+J[80]*2+J[79]);if(!(i[256]&32))K~(32+J[40]*8+J[43]*4+J[29]*2+J[27]);K 255;}
K i[a&511];}Q 12 ...14:a&=16383;if(w)r4[a]=x;K r4[a];}}z r(Z a=H){K me(a,0,0);}void w(z x,Z a=H){me(a,x,1);}void f
(z M,I Z,I N,I H,I C){F=(F&M)|(!Z*128+N*64+H*32+C*16);}z y(){K r(P++);}Z Y(Z&x=P){u=r(x++);K r(x++)*256+u;}void ph(Z x
){w(x>>8,--S);w(x,--S);T();}

I main(){r1=(r0=(z*)mmap(0,1048576,PROT_READ,MAP_SHARED,open("rom.gb",O_RDONLY),0))+32768;s=open("rom.sav",
O_CREAT|O_RDWR,0666);ftruncate(s,32768);r3=r2=(z*)mmap(0,32768,PROT_READ|PROT_WRITE,MAP_SHARED,s,0);O=145;
DI=44032;SDL_Init(SDL_INIT_VIDEO);SDL_Renderer*Sr=SDL_CreateRenderer(SDL_CreateWindow("pokegb",0,0,800,720,
SDL_WINDOW_SHOWN),-1,0/*SDL_RENDERER_PRESENTVSYNC*/);SDL_Texture*St=SDL_CreateTexture(Sr,SDL_PIXELFORMAT_RGBA32,
SDL_TEXTUREACCESS_STREAMING,160,144);J=SDL_GetKeyboardState(0);

while(1){C_=C;if(IM&IF&i[511]){IF=h=IM=0;C+=8;c=1;U=64;G C_;}else if(h)T();else switch(o=y()){
Q 0:B W(1)*E0[o>>4]=Y();B W(2)w(A,*E1[o>>4]);H+=HA[m/16];B W(3)(*E0[o>>4])++;T();B Y1(4)u=++(*p);G I_;Q 52:w(u=r()+
1);I_:f(16,u,0,!(u&15),0);B Y1(5)u=--(*p);G D_;Q 53:w(u=r()-1);D_:f(16,u,1,u%16==15,0);B Y1(6)*p=y();B Q 7:A+=A+(c=
A>>7);G F_;W(9)M=E0[o>>4];f(128,1,0,H%4096+*M%4096>4095,H+*M>65535);H+=*M;T();B W(10)A=r(*E1[o>>4]);H+=HA[m/16];B
W(11)(*E0[o>>4])--;T();B Q 15:A=(c=A&1)*128+A/2;G F_;Q 23:c=A>>7;A+=A+F/16%2;F_:f(0,1,0,0,c);B Y5(32,24)u=y();if(c)P+=(
int8_t)u,T();B Q 39:c=u=0;if(F&32||(!(F&64)&&A%15>9))u=6;if(F&16||(!(F&64)&&A>153))u|=96,c=1;f(65,A+=(F&64)?-u:u,0,0,c);
B Q 47:A=~A;f(129,1,1,1,0);B Q 54:w(y());B Q 55:Q 63:f(128,1,0,0,o==55?1:!(F&16));B Y1(70)*p=r();B Y4(64)*e0[m/8]=*e0[
o&7];B X(112)w(*p);B Q 118:h=1;B D(128)n=c=0;G A_;D(136)n=0;c=F/16%2;G A_;D(144)c=1;G SUB;D(152)c=!(F/16%2);SUB:
n=1;x=~x;A_:f(0,u=A+x+c,n,(A%16+x%16+c>15)^n,(A+x+c>255)^n);A=u;B D(160)f(0,A&=x,0,1,0);B D(168)f(0,A^=x,0,0,0);B D
(176)f(0,A|=x,0,0,0);B D(184)f(0,A!=x,1,A%16<x%16,A<x);B Q 217:c=IM=1;G R_;Y5(192,201)R_:T();if(c)P=Y(S);B W(193)E[m
>>4]=Y(S);B Y5(194,195)U=Y();if(c)P=U,T();B Y5(196,205)U=Y();C_:if(c)ph(P),P=U;B W(197)ph(E[m>>4]);B Q 203:
switch(o=y()){X(0)*p+=*p+(c=*p/128%2);G N;X(8)*p=(c=*p&1)*128+*p/2;G N;Q 14:u=r();c=u&1;w(u=c*128+u/2);G F0;X(16)c=*p
>>7;*p=*p*2+F/16%2;G N;X(24)c=*p&1;*p=*p/2+((F*8)&128);G N;X(32)c=*p/128%2;*p*=2;G N;X(40)c=*p&1;*p=(int8_t)*p>>1;G
N;X(48)f(0,*p=*p*16+*p/16,0,0,0);B Q 54:u=r();c=0;w(u=u*16+u/16);F0:f(0,u,0,0,c);B X(56)c=*p&1;*p/=2;N:f(0,*p,0,0,c);
B Y0(70)u=r();G B_;Y2(64)u=*p;B_:f(16,u&(1<<m/8),0,1,0);B Y0(134)w(r()&~(1<<m/8));B Y2(128)*p&=~(1<<m/8);B Y0(198)
w(r()|(1<<m/8));B Y2(192)*p|=1<<m/8;B}B Q 224:Q 226:w(A,65280+(o==224?y():*e));B Q 233:P=H;B Q 234:w(A,Y());B Q 240:Q
250:A=r(o==240?65280|y():Y());B Q 243:Q 251:IM=o!=243;B Q 248:u=y();f(0,1,0,(z)S+u>255,S%16+u%16>15);H=S+(int8_t)u;T();
B Q 249:S=H;T();B}for(DI+=C-C_;C_++!=C;)if(O&128){if(++d==1&&L==144)IF|=1;if(d==456){if(L<144)for(s=160;--s
>=0;){z w=O&32&&L>=i[330]&&s>=i[331]-7,mx=w?s-i[331]+7:s+i[323],my=w?L-i[330]:L+i[322];Z t=v[((O&(w?64:8)?7:
6)<<10)+my/8*32+mx/8],p=327;mx=(mx^7)&7;z*d=&v[(O&16?t:256+(int8_t)t)*16+my%8*2],c=(d[1]>>mx)%2*2+(*d>>mx)%2
;if(O&2)for(z*o=i;o<i+160;o+=4){z dx=s-o[1]+8,dy=L-*o+16;if(dx<8&&dy<8){dx^=o[3]&32?0:7;d=&v[o[2]*16+(dy^(o[3]&64?
7:0))*2];z g=(d[1]>>dx)%2*2+(*d>>dx)%2;if(!((o[3]&128)&&c)&&g){c=g;p+=1+!!(o[3]&8);B}}}fb[L*160+s]=pal[(i[p]>>(2*c))&
3];}if(L==144){void*ps;SDL_LockTexture(St,0,&ps,&s);for(t=144;--t>=0;)memcpy((z*)ps+t*s,fb+t*160,640);SDL_UnlockTexture(
St);SDL_RenderCopy(Sr,St,0,0);SDL_RenderPresent(Sr);SDL_Delay(0);SDL_Event e;while(SDL_PollEvent(&e))if(e.type==SDL_QUIT
)K 0;}L=(L+1)%154;d=0;}}else L=d=0;}}
