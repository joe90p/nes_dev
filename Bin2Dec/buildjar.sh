mkdir -p bin
rm bin/*
javac -d bin src/*.java
cd bin
jar cmvf ../META-INF/MANIFEST.MF bintodec.jar *.class
