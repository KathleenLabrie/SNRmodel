       subroutine testf(a,nx,ny)
       integer nx,ny
       real a(nx,ny)
       
       do 11 j=1,ny
       do 10 i=1,nx
         write(*,*) a(i,j)
10     continue
11     continue
       return
       end
