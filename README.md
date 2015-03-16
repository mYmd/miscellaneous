miscellaneous
=============

miscellaneous C++ files

functor_overload.hpp  
test_functor_overload.cpp(サンプル)  
***********************************************************************  
  オーバーロードされたファンクタを operator + を使ってその場で作る関数  
  シグネチャ（のパターン）を指定することによって複数のファンクタを意識的に選択する  
  （各ファンクタの元のシグネチャをそのまま引き継ぐ訳ではない）  
  シグネチャのパターンとは単に引数の型を指定するだけでなく、引数が満たす制約を指定するもの  
  （制約とは、std::is_integral のようなメタ述語による）  
 ----------------------------------------------------------------------  
  以下の形の式を + 演算子で任意個つなげてオーバーロードされたファンクタを作れる  
      gen<引数型 または cond<引数型が満たす制約>...>(ファンクタ)  
  例）     gen<int, int, int>(func1)  
        +  gen<cond<std::is_pointer>, char, cond<std::is_integral>>(func2)  
        +  gen<double, int*, cond<std::is_integral, false>>(func3);  
***********************************************************************  

mbind.hpp  
test_mbind.cpp(サンプル)  
テンプレートパラメータの部分束縛  
