function B = fast_blockident(M,nr,nc)
% B = fast_blockident(M,nr,nc)
%
% Puts M on diagonal. There will be nr X nc submatrices
% the same size as M, those off the diagonal will be 0.
% This is refered to as the "block idenity matrix" because
% each matrix on the diagonal is idenitcal.

[Mnr Mnc] = size(M);
Bnr = nr*Mnr;
Bnc = nc*Mnc;

B0 = zeros(Mnr,Bnc);
B0(:,1:Mnc) = M;

B = B0;
for n = 2:nr;
  Btmp = fast_mshift(B0, [0 (n-1)*Mnc], 0);
  B = [B; Btmp];
end

return;
