# B1:
on yaminocellist M1 Mac, run: 

```bash
    cmake .. -DCMAKE_C_COMPILER=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang \
         -DCMAKE_CXX_COMPILER=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++;make -j8
```

the resolution is adjusted in vis.mac;

Some useful commands:
```bash
    /control/saveHistory mysession.mac
    /vis/viewer/zoom <MULTIPLE>
    /vis/viewer/panTo <x> <y> <unit>
    /vis/viewer/set/viewpointThetaPhi 120 150
    /tracking/verbose 2
```

For DAWN users:
```bash
    /vis/open DAWNFILE
    /vis/scene/create
    /vis/scene/add/volume
    /vis/scene/add/trajectories
    /vis/scene/add/hits
    /vis/sceneHandler/attach
    /tracking/storeTrajectory 1
    /vis/scene/endOfEventAction accumulate
    /run/beamOn 10
```