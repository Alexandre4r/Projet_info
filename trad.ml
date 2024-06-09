let f(i,j) = 
    (!i)+(!j);
;;
let g(i,j) = 
    f(ref ((!i)), ref ((!j)))*(!i);
;;
Printf.printf "UXU %d"  (g(ref (4), ref ( 5)));;
let i = ref 0;; 
Printf.printf "%dtest"  (!i);;
while((!i) = 4) do 
    i := (!i)+1;
    if((!i)=3) then begin 
        for j = 0 to 10 do
            Printf.printf "%dtest"  (!i);
        done;
    end
done;;
if((!i)=3) then begin 
    Printf.printf "%dtest"  (!i);
end
else  begin 
    i := (!i)+1;
end
let funct(i,j) = 
    (!i)+(!j)
;;
(*erika miam*)(*Test****)