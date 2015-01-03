program matrixmul
  use timings_m
  implicit none

  integer, parameter :: RK = selected_real_kind(10,2)
  real(kind=RK), parameter :: PRECISION = 1E-8
  integer :: N=100
  real(kind=RK),pointer :: A(:,:), B(:,:), C(:,:), Z(:,:)

  type(timing_t) :: timing
  
  integer :: argc
  character ( len = 80 ) arg
  integer :: i,j
  
  ! read size from program arguments
  argc = iargc()
  if (argc>0) then
    call getarg(1,arg)
    read( arg, '(i10)' ) N
  end if

  ! initialize matrices
  allocate(A(N,N), B(N,N), C(N,N), Z(N,N))
  do i=1,N
  	do j=1,N
     A(i,j) = real(i + j)
     B(i,j) = real(i - j)
    end do
  end do

  print '(I0,$)', N

  !corect answer
  Z = matmul(A, B)
    
  ! blas
  timing = timing_start()
  call dgemm('N', 'N', N, N, N, 1.0_RK, &
        A, N, B, N, 0.0_RK, C, N)
  call timing_finish(timing)
  call assert(Z, C)
  print 20, timing_getTime(timing)

  print*, ""

20 format (E17.9,$)

contains
  
  subroutine assert(u,uc)
    real(kind=RK),intent(in) :: u(:,:), uc(:,:)
    real(kind=RK) :: p
    integer :: N
    N = size(u,1)


    p = dot_product(reshape(u-uc, (/N*N/)), reshape(u-uc, (/N*N/)))
    if (p > PRECISION) then
       print *, p
       stop "Wrong answer"
    end if
  end subroutine assert

end program matrixmul
