function isas = fmri_isavgstruct()
% isas = fmri_isavgstruct()
%
% Creates a structure for inter-subject averaging.
%
% $Id: fmri_isavgstruct.m,v 1.1 2003/03/04 20:47:39 greve Exp $
%


isas = struct(...
  'hdrstem', [], ...
  'hdrgroupid', 0, ...
  'sigfile', [], ...
  'sigformat', 'lnp', ...
  'havg',  [], ...
  'p', [],...
  'w', [] );


