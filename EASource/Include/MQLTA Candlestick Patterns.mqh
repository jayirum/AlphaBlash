#property copyright     "EarnForex.com"
#property link          "https://www.earnforex.com/"
#property version       "1.00"
#property strict


bool IsDojiNeutral(int Shift=0){
   double Shadow=High[Shift]-Low[Shift];
   double Body=MathAbs(Close[Shift]-Open[Shift]);
   if(Body<Shadow*0.05 && !IsDojiGravestone(Shift) && !IsDojyDragonfly(Shift)) return true;
   else return false;
}


bool IsDojyDragonfly(int Shift=0){
   double Shadow=High[Shift]-Low[Shift];
   double Body=MathAbs(Close[Shift]-Open[Shift]);
   if(Body<Shadow*0.05 && Close[Shift]>High[Shift]-Shadow*0.05) return true;
   else return false;
}


bool IsDojiGravestone(int Shift=0){
   double Shadow=High[Shift]-Low[Shift];
   double Body=MathAbs(Close[Shift]-Open[Shift]);
   if(Body<Shadow*0.05 && Close[Shift]<Low[Shift]+Shadow*0.05) return true;
   else return false;
}


bool IsSpinningTopBullish(int Shift=0){
   double Shadow=High[Shift]-Low[Shift];
   double Body=MathAbs(Close[Shift]-Open[Shift]);
   if(Open[Shift]<Close[Shift] && Close[Shift]<High[Shift]-Shadow*0.30 && Open[Shift]>Low[Shift]+Shadow*0.30 && Body<Shadow*0.4 && Body>Shadow*0.05) return true;
   else return false;
}


bool IsSpinningTopBearish(int Shift=0){
   double Shadow=High[Shift]-Low[Shift];
   double Body=MathAbs(Close[Shift]-Open[Shift]);
   if(Open[Shift]>Close[Shift] && Open[Shift]<High[Shift]-Shadow*0.30 && Close[Shift]>Low[Shift]+Shadow*0.30 && Body<Shadow*0.4 && Body>Shadow*0.05) return true;
   else return false;
}


bool IsMarubozuUp(int Shift=0){
   double Shadow=High[Shift]-Low[Shift];
   double Body=MathAbs(Close[Shift]-Open[Shift]);
   if(Open[Shift]<Close[Shift] && Close[Shift]>High[Shift]-Shadow*0.02 && Open[Shift]<Low[Shift]+Shadow*0.02 && Body>Shadow*0.95) return true;
   else return false;
}


bool IsMarubozuDown(int Shift=0){
   double Shadow=High[Shift]-Low[Shift];
   double Body=MathAbs(Close[Shift]-Open[Shift]);
   if(Open[Shift]>Close[Shift] && Close[Shift]<High[Shift]+Shadow*0.02 && Open[Shift]>Low[Shift]-Shadow*0.02 && Body>Shadow*0.95) return true;
   else return false;
}


bool IsHammer(int Shift=0){
   double Shadow=High[Shift]-Low[Shift];
   double Body=MathAbs(Close[Shift]-Open[Shift]);
   if(Open[Shift]<Close[Shift] && Close[Shift]>High[Shift]-Shadow*0.05 && Body<Shadow*0.4 && Body>Shadow*0.1) return true;
   else return false;
}


bool IsHangingMan(int Shift=0){
   double Shadow=High[Shift]-Low[Shift];
   double Body=MathAbs(Close[Shift]-Open[Shift]);
   if(Open[Shift]>Close[Shift] && Open[Shift]>High[Shift]-Shadow*0.05 && Body<Shadow*0.4 && Body>Shadow*0.1) return true;
   else return false;
}


bool IsInvertedHammer(int Shift=0){
   double Shadow=High[Shift]-Low[Shift];
   double Body=MathAbs(Close[Shift]-Open[Shift]);
   if(Open[Shift]<Close[Shift] && Open[Shift]<Low[Shift]+Shadow*0.05 && Body<Shadow*0.4 && Body>Shadow*0.1) return true;
   else return false;
}


bool IsShootingStar(int Shift=0){
   double Shadow=High[Shift]-Low[Shift];
   double Body=MathAbs(Close[Shift]-Open[Shift]);
   if(Open[Shift]>Close[Shift] && Close[Shift]<Low[Shift]+Shadow*0.05 && Body<Shadow*0.4 && Body>Shadow*0.1) return true;
   else return false;
}


bool IsBullishEngulfing(int Shift=0){
   int i=Shift;
   int j=i+1;
   double ShadowPrev=High[j]-Low[j];
   double BodyCurr=MathAbs(Close[i]-Open[i]);
   if(IsDojiNeutral(j)) return false;
   if(Close[j]<Open[j] && Close[i]>Open[i] && Close[i]>High[j] && BodyCurr>ShadowPrev) return true;
   else return false;
}


bool IsBearishEngulfing(int Shift=0){
   int i=Shift;
   int j=i+1;
   double ShadowPrev=High[j]-Low[j];
   double BodyCurr=MathAbs(Close[i]-Open[i]);
   if(IsDojiNeutral(j)) return false;
   if(Close[j]>Open[j] && Close[i]<Open[i] && Close[i]<Low[j] && BodyCurr>ShadowPrev) return true;
   else return false;
}


bool IsTweezerTop(int Shift=0){
   int i=Shift;
   int j=i+1;
   double ShadowPrev=High[j]-Low[j];
   double BodyCurr=MathAbs(Close[i]-Open[i]);
   if(IsInvertedHammer(j) && IsShootingStar(i) && 
      ((High[j]<High[i]*1.05 && High[j]>High[i]*0.95) ||
      (High[i]<High[j]*1.05 && High[i]>High[j]*0.95)) &&
      ((Open[j]<Close[i]*1.05 && Open[j]>Close[i]*0.95) ||
      (Close[i]<Open[j]*1.05 && Close[i]>Open[j]*0.95))
      ) return true;
   else return false;
}


bool IsTweezerBottom(int Shift=0){
   int i=Shift;
   int j=i+1;
   double ShadowPrev=High[j]-Low[j];
   double BodyCurr=MathAbs(Close[i]-Open[i]);
   if(IsHangingMan(j) && IsHammer(i) && 
      ((Low[j]<Low[i]*1.05 && Low[j]>Low[i]*0.95) ||
      (Low[i]<Low[j]*1.05 && Low[i]>Low[j]*0.95)) &&
      ((Open[j]<Close[i]*1.05 && Open[j]>Close[i]*0.95) ||
      (Close[i]<Open[j]*1.05 && Close[i]>Open[j]*0.95))
      ) return true;
   else return false;
}


bool IsThreeWhiteSoldiers(int Shift=0){
   int i=Shift;
   int j=i+1;
   int k=j+1;
   double ShadowI=High[i]-Low[i];
   double ShadowJ=High[j]-Low[j];
   double ShadowK=High[k]-Low[k];
   double BodyI=MathAbs(Close[i]-Open[i]);
   double BodyJ=MathAbs(Close[j]-Open[j]);
   double BodyK=MathAbs(Close[k]-Open[k]);
   if(Close[i]>Open[i] && Close[j]>Open[j] && Close[k]>Open[k] && BodyI>ShadowI*0.5 && BodyJ>ShadowJ*0.5 && BodyK>ShadowK*0.5 && BodyJ<BodyI && BodyK<BodyJ) return true;
   else return false;
}


bool IsThreeCrows(int Shift=0){
   int i=Shift;
   int j=i+1;
   int k=j+1;
   double ShadowI=High[i]-Low[i];
   double ShadowJ=High[j]-Low[j];
   double ShadowK=High[k]-Low[k];
   double BodyI=MathAbs(Close[i]-Open[i]);
   double BodyJ=MathAbs(Close[j]-Open[j]);
   double BodyK=MathAbs(Close[k]-Open[k]);
   if(Close[i]<Open[i] && Close[j]<Open[j] && Close[k]<Open[k] && BodyI>ShadowI*0.5 && BodyJ>ShadowJ*0.5 && BodyK>ShadowK*0.5 && BodyJ<BodyI && BodyK<BodyJ) return true;
   else return false;
}


bool IsThreeInsideUp(int Shift=0){
   int i=Shift;
   int j=i+1;
   int k=j+1;
   double ShadowI=High[i]-Low[i];
   double ShadowJ=High[j]-Low[j];
   double ShadowK=High[k]-Low[k];
   double BodyI=MathAbs(Close[i]-Open[i]);
   double BodyJ=MathAbs(Close[j]-Open[j]);
   double BodyK=MathAbs(Close[k]-Open[k]);
   if(Close[i]>Open[i] && Close[j]>Open[j] && Close[k]<Open[k] && 
      Close[j]<Open[k] && Close[j]>Close[k]+BodyK/4 && Close[i]>High[k] &&  
      BodyI>ShadowI/2 && BodyJ>ShadowJ/2 && BodyK>ShadowK/2) return true;
   else return false;
}


bool IsThreeInsideDown(int Shift=0){
   int i=Shift;
   int j=i+1;
   int k=j+1;
   double ShadowI=High[i]-Low[i];
   double ShadowJ=High[j]-Low[j];
   double ShadowK=High[k]-Low[k];
   double BodyI=MathAbs(Close[i]-Open[i]);
   double BodyJ=MathAbs(Close[j]-Open[j]);
   double BodyK=MathAbs(Close[k]-Open[k]);
   if(Close[i]<Open[i] && Close[j]<Open[j] && Close[k]>Open[k] && 
      Close[j]>Open[k] && Close[j]<Close[k]-BodyK/4 && Close[i]<Low[k] && 
      BodyI>ShadowI/2 && BodyJ>ShadowJ/2 && BodyK>ShadowK/2) return true;
   else return false;
}


bool IsMorningStar(int Shift=0){
   int i=Shift;
   int j=i+1;
   int k=j+1;
   double ShadowI=High[i]-Low[i];
   double ShadowJ=High[j]-Low[j];
   double ShadowK=High[k]-Low[k];
   double BodyI=MathAbs(Close[i]-Open[i]);
   double BodyJ=MathAbs(Close[j]-Open[j]);
   double BodyK=MathAbs(Close[k]-Open[k]);
   if(Close[i]>Open[i] && Close[k]<Open[k] && Close[i]>Open[k]-BodyK/2 && 
      (IsDojiNeutral(j) || IsSpinningTopBullish(j)) && !IsDojiNeutral(k)) return true;
   else return false;
}


bool IsEveningStar(int Shift=0){
   int i=Shift;
   int j=i+1;
   int k=j+1;
   double ShadowI=High[i]-Low[i];
   double ShadowJ=High[j]-Low[j];
   double ShadowK=High[k]-Low[k];
   double BodyI=MathAbs(Close[i]-Open[i]);
   double BodyJ=MathAbs(Close[j]-Open[j]);
   double BodyK=MathAbs(Close[k]-Open[k]);
   if(Close[i]<Open[i] && Close[k]>Open[k] && Close[i]<Open[k]+BodyK/2 && 
      (IsDojiNeutral(j) || IsSpinningTopBearish(j)) && !IsDojiNeutral(k)) return true;
   else return false;
}
