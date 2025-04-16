int eqv(pointer a, pointer b) {
     if (is_string(a)) {
          if (is_string(b))
               return (strvalue(a) == strvalue(b));
          else
               return (0);
     } else if (is_number(a)) {
          if (is_number(b)) {
               if (num_is_integer(a) == num_is_integer(b))
                    return num_eq(nvalue(a),nvalue(b));
          }
          return (0);
     } else if (is_character(a)) {
          if (is_character(b))
               return charvalue(a)==charvalue(b);
          else
               return (0);
     } else if (is_port(a)) {
          if (is_port(b))
               return a==b;
          else
               return (0);
     } else if (is_proc(a)) {
          if (is_proc(b))
               return procnum(a)==procnum(b);
          else
               return (0);
     } else {
          return (a == b);
     }
}