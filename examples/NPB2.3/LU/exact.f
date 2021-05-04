c---------------------------------------------------------------------
c---------------------------------------------------------------------

      subroutine exact( i, j, k, u000ijk )

c---------------------------------------------------------------------
c---------------------------------------------------------------------

c---------------------------------------------------------------------
c
c   compute the exact solution at (i,j,k)
c
c---------------------------------------------------------------------

      implicit none

      include 'applu.incl'

c---------------------------------------------------------------------
c  input parameters
c---------------------------------------------------------------------
      integer profiler(2)
      save profiler
      integer i, j, k
      double precision u000ijk(*)

c---------------------------------------------------------------------
c  local variables
c---------------------------------------------------------------------
      integer m
      double precision xi, eta, zeta

      call TAU_PROFILE_TIMER(profiler, 'exact');
      call TAU_PROFILE_START(profiler);
      xi  = ( dble ( i - 1 ) ) / ( nx0 - 1 )
      eta  = ( dble ( j - 1 ) ) / ( ny0 - 1 )
      zeta = ( dble ( k - 1 ) ) / ( nz - 1 )


      do m = 1, 5
         u000ijk(m) =  ce(m,1)
     >        + ce(m,2) * xi
     >        + ce(m,3) * eta
     >        + ce(m,4) * zeta
     >        + ce(m,5) * xi * xi
     >        + ce(m,6) * eta * eta
     >        + ce(m,7) * zeta * zeta
     >        + ce(m,8) * xi * xi * xi
     >        + ce(m,9) * eta * eta * eta
     >        + ce(m,10) * zeta * zeta * zeta
     >        + ce(m,11) * xi * xi * xi * xi
     >        + ce(m,12) * eta * eta * eta * eta
     >        + ce(m,13) * zeta * zeta * zeta * zeta
      end do

      call TAU_PROFILE_STOP(profiler);
      return
      end
