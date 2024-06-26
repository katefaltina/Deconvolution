<p>Задача деконволюции свёртки является одной из самых распространённых задач на практике цифровой обработки сигналов.</p>
<p>Особенно когда имеем дело с линейными системами, в которых выходной сигнал вычисляется как линейная свёртка входного сигнала с импульсной характеристикой линейной системы. Такую задачу называют прямой или задачей распространения, когда с помощью линейной операции свёртки находят выходной сигнал, зная входной сигнал и импульсную характеристику линейной системы.</p> 
<p>Если же даны импульсная характеристика и выходной сигнал, и требуется восстановить входной сигнал, то такая задача называется обратной. Она относится к некорректным задачам и для её решения требуется применить какой-нибудь способ регуляризации или доопределить задачу.</p>
<p>В данной программе реализовано обращение циклической свёртки с помощью метода максимума энтропии в качестве регуляризирующего требования к решению.</p>
<p>В данной задаче решаем не только обратную задачу, но и задачу прямого распространения, чтобы создать данные, на основе которых можно было бы решить обратную задачу. Входной сигнал представляет собой сумму трех гауссовых куполов.</p>
<p>!! Задача представляет собой задачу многомерной оптимизации. Решение будет уточняться на каждой итерации, причём таких итераций в зависимости от требуемой точности может быть достаточно много. Поэтому в этой задаче стоит строить сигналы длиной не более 50 отсчётов.</p>

<img src="https://github.com/katefaltina/Deconvolution/assets/125446752/fbc2b91a-8b6e-4377-a5f1-c3ffdfc5b301">
