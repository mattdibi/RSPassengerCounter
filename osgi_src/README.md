## User guide

*1) Copy the content of this folder inside the target platform*

```sh
$ scp -r osgi_src/ root@<target platform IP>:
``` 

*2) Run the OSGi framework*

```sh
$ cd equinox_jars/
$ java -jar org.eclipse.osgi_3.6.1.R36x_v20100806.jar -console
``` 

*3) Install your bundle (in this example the HelloWorld bundle) and start it*

```sh
osgi> install file:../com.Mattia.HelloWorld_1.0.0.201704181142.jar
osgi> start <bundle id>
``` 

### Runtime commands
```
osgi> start <bundle id>: start the bundle
osgi> stop <bundle id>: stop the bundle
osgi> ss: list all bundle
```
