      subroutine ShhNuisancePythiaItOkay()
          integer, parameter :: stdout = 6
          integer, parameter :: stderr = 0
          character(len=*), parameter :: nullfile="/dev/null"
          ! write(6,"(A)") "[INFO]: Attempting to quiten stdout..."
          open(unit=stdout, file=nullfile, status="old")
          open(unit=stderr, file=nullfile, status="old")
      end subroutine ShhNuisancePythiaItOkay
      subroutine CanIHazNuisancePythia()
          integer, parameter :: stdout = 6
          integer, parameter :: stderr= 0
          character(len=*), parameter :: nullfile="/dev/stdout"
          open(unit=stdout, file=nullfile, status="old")
          open(unit=stderr, file=nullfile, status="old")
      end subroutine CanIHazNuisancePythia

