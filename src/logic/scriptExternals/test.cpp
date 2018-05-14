

int square(int x, int y)
{
  return x*y;
}


void test(int y)
{



  auto myFunction = [=](int x){
    return x*y;
  };




  myFunction(2);
  myFunction(5);
  myFunction(2);
  myFunction(2);
  myFunction(2);
  myFunction(7);
  myFunction(2);
  myFunction(1);
  square(5, y);




}




