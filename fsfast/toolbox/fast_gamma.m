function [h, t] = fast_gamma(delta, tau, TER, TPreStim, TimeWindow)
%
% [h t] = fast_gamma(delta, tau, TER, TPreStim, TimeWindow)
%
% Computes the gamma function (from Dale and Buckner, 1997):
%
% h(t>delta)  = ((t-delta)/tau)^2 * exp(-(t-delta)/tau)
% h(t<=delta) = 0;
%
% The waveform is scaled so that the peak = 1.0
% Sample parameters (from D&B): delta = 2.25 sec, tau = 1.25 sec
%
% See also fast_gamma and fmri_hemodyn.
%
% $Id: fast_gamma.m,v 1.1 2003/03/04 20:47:38 greve Exp $
%


h = [];

if(nargin ~= 5)
  msg = 'USAGE: [h t] = fast_gamma(delta, tau, TER, TPreStim, TimeWindow)\n';
  qoe(msg);error(msg);
end

NWindow = round(TimeWindow/TER);
t = TER * [0:NWindow-1] - TPreStim;
t = reshape1d(t); % Column vector

r = (t - delta)./tau ;
h = ( ( r.^2) .* exp(-r) );
i0 = find(t<delta);
h(i0) = zeros(size(t(i0)));

% scale max to 1 %
h = h/max(h);

return;
