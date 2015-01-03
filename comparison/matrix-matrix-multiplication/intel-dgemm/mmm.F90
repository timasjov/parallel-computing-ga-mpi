program matrixmul
  use timings_m
  implicit none

  integer, parameter :: RK = selected_real_kind(10,2)
  real(kind=RK), parameter :: PRECISION = 1E-8
  integer :: N=2500
  real(kind=RK),pointer :: A(:,:), B(:,:), C(:,:), Z(:,:)

  type(timing_t) :: timing
  
  integer :: argc
  character ( len = 80 ) arg
  integer :: i,j,t
  
  ! read size from program arguments
  argc = iargc()
  if (argc>0) then
    call getarg(1,arg)
    read( arg, '(i10)' ) N
  end if

  timing = timing_start()
  do t=1,1000
	  ! initialize matrices
	  allocate(A(N,N), B(N,N), C(N,N), Z(N,N))
	  do i=1,N
		do j=1,N
		 A(i,j) = real(i + j)
		 B(i,j) = real(i * j)
		end do
	  end do

	  !corect answer
	  Z = matmul(A, B)
		
	  ! blas
	  call dgemm('N', 'N', N, N, N, 1.0_RK, &
			A, N, B, N, 0.0_RK, C, N)
  end do
  call timing_finish(timing)
  print 20, timing_getTime(timing)


20 format (E17.9,$)

end program matrixmul
