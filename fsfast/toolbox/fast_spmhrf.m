function h = fast_spmhrf(t,ttp1,tau1,ttp2,tau2,c)
%
% h = fast_spmhrf(t)
% h = fast_spmhrf(t,ttp1,tau1,ttp2,tau2,c)
%
% SPM99 hemodynamic response function. 
%
% ttp1 = time-to-peak of the positive response 
% tau1 = dispersion of the positive response 
% ttp2 = time-to-peak of the negative response 
% tau2 = dispersion of the negative response 
% c = amplitude of negative with respect to the positive
%
% This will give exactly the same result as SPMs spm_hrf.m under the
% default conditions. In general, 
%    p = [(tt1+1) (ttp2+1) tau1 tau2 1/c 0]
% Then, 
%    h = fast_spm_hrf(TR,p);
% This will yeild the same result when tau1=tau2=1, t = [0:TR:32], and
%    h = fast_spmhrf(t,ttp1,tau1,ttp2,tau2,c);

h = [];

if(nargin ~= 1 & nargin ~= 6)
  fprintf('USAGE: h = fast_spmhrf(t,ttp1,tau1,ttp2,tau2,c)\n');
  return;
end

if(nargin == 1)
  % These are the default values %
  tau1 = 1;
  tau2 = 1;
  ttp1 = 5*tau1;  % 6 %
  ttp2 = 15*tau1; % 16 %
  c  = 1/6;
end
  
a1 = ttp1/tau1;
a2 = ttp2/tau2;

h1 = (t/ttp1).^a1 .* exp(-(t-ttp1)/tau1);
h1 = h1/sum(h1);
h2 = (t/ttp2).^a2 .* exp(-(t-ttp2)/tau2);
h2 = h2/sum(h2);
h = h1 - c*h2;

h = h/sum(h);

return;

