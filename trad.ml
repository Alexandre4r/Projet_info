let i = ref 0;; 
Printf.printf "%dtest" (!i);;
while((!i) <> 4) do 
i := (!i)+1;
done;;
if((!i)=3) then begin 
Printf.printf "%dtest" (!i);
 end;;
