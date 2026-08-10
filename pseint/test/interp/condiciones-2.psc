proceso condiciones
  s1<-"hola"
  s2<-"chau"
  n1<-10
  n2<-5.5
  n3<-1/2
  
  escribir "V ",n1>n2 y s1>s2
  escribir "V ",n1>n2 o s1<s2
  escribir "V ",n1<n2 o s1>s2
  escribir "F ",n1<n2 o s1<s2
  escribir "F ",n1>n2 y s1<s2
  escribir "F ",n1<n2 y s1>s2
  escribir "F ",n1<n2 = s1>s2
  escribir "V ",n1>n2 = s1>s2
  escribir "F ",n1<n2 y s2<s1 o n1>n2 y s2>s1
  escribir "F ",(n1>n2 y s2>s1 o n1<n2) y s2<s1
  escribir "F ",n1>n2 y (s2>s1 o n1<n2 y s2<s1)
  escribir "V ",(n1>n2 y s2>s1 o n1<n2) o s2<s1
  escribir "V ",n1>n2 o (s2>s1 o n1<n2 y s2<s1)
  escribir "F ",(n1>n2 y s2>s1) o (n1<n2 y s2<s1)
  escribir "F ",n1>n2 y (s2>s1 o n1<n2) y s2<s1
finproceso
