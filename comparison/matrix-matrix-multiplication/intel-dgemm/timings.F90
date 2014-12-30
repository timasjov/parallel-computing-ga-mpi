module timings_m
#ifdef _OPENMP
  use omp_lib
#endif
  implicit none

#ifdef _OPENMP
  type Timing_t
     private
     real(kind=8) :: start, end
  end type Timing_t

#else
  type Timing_t
     private
     integer(kind=8) :: start, end, rate
  end type Timing_t
#endif

contains

#ifdef _OPENMP
  type(Timing_t) function timing_start()
    timing_start%start = omp_get_wtime()
  end function timing_start

  subroutine timing_finish(t)
    type(Timing_t),intent(inout) :: t
    t%end = omp_get_wtime()
  end subroutine timing_finish

  real(kind=8) function timing_getTime(t)
    type(Timing_t),intent(in) :: t
    timing_getTime = (t%end-t%start)
  end function timing_getTime

#else  
  type(Timing_t) function timing_start()
    call system_clock(timing_start%start, timing_start%rate)
  end function timing_start

  subroutine timing_finish(t)
    type(Timing_t),intent(inout) :: t
    call system_clock(t%end)
  end subroutine timing_finish

  real(kind=8) function timing_getTime(t)
    type(Timing_t),intent(in) :: t
    timing_getTime = (t%end-t%start)/real(t%rate)
  end function timing_getTime
#endif

end module timings_m
