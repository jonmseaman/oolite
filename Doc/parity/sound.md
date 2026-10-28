# Parity findings: Sound and music

Legend: ✅ equivalent · ⚠️ intentional divergence · ❌ suspected behaviour difference · ➖ removed/replaced.

Each section: method coverage table (every upstream method / C function → C++ counterpart → verdict), then differences with `upstream:line` ↔ `oolite-dev:line`, severity and intent.


## OOALBufferedSound

<a id="ooalbufferedsound"></a>
Upstream `src/Core/OOALBufferedSound.m` ↔ `Core/OOALBufferedSound.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOALBufferedSound dealloc]` (34) | | |
| `-[OOALBufferedSound name]` (43) | | |
| `-[OOALBufferedSound initWithDecoder:]` (48) | | |
| `-[OOALBufferedSound soundBuffer]` (76) | | |

**Differences:**

- *pending*


## OOALMusic

<a id="ooalmusic"></a>
Upstream `src/Core/OOALMusic.m` ↔ `Core/OOALMusic.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOMusic allocWithZone:]` (36) | | |
| `-[OOMusic dealloc]` (41) | | |
| `-[OOMusic initWithContentsOfFile:]` (50) | | |
| `-[OOMusic name]` (64) | | |
| `-[OOMusic setMusicGain:]` (69) | | |
| `-[OOMusic musicGain]` (76) | | |
| `-[OOMusic playLooped:]` (83) | | |
| `-[OOMusic musicSoundSource]` (98) | | |
| `-[OOMusic isPlaying]` (103) | | |
| `-[OOMusic stop]` (108) | | |

**Differences:**

- *pending*


## OOALSound

<a id="ooalsound"></a>
Upstream `src/Core/OOALSound.m` ↔ `Core/OOALSound.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOSound setUp]` (46) | | |
| `+[OOSound setMasterVolume:]` (61) | | |
| `+[OOSound masterVolume]` (75) | | |
| `-[OOSound init]` (84) | | |
| `-[OOSound initWithContentsOfFile:]` (91) | | |
| `-[OOSound initWithDecoder:]` (124) | | |
| `-[OOSound name]` (130) | | |
| `+[OOSound update]` (136) | | |
| `+[OOSound isSoundOK]` (143) | | |
| `-[OOSound soundBuffer]` (148) | | |
| `-[OOSound soundIncomplete]` (154) | | |
| `-[OOSound rewind]` (159) | | |

**Differences:**

- *pending*


## OOALSoundChannel

<a id="ooalsoundchannel"></a>
Upstream `src/Core/OOALSoundChannel.m` ↔ `Core/OOALSoundChannel.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOSoundChannel init]` (43) | | |
| `-[OOSoundChannel dealloc]` (61) | | |
| `-[OOSoundChannel update]` (68) | | |
| `-[OOSoundChannel getNextSoundBuffer]` (89) | | |
| `-[OOSoundChannel setDelegate:]` (115) | | |
| `-[OOSoundChannel next]` (120) | | |
| `-[OOSoundChannel setNext:]` (125) | | |
| `-[OOSoundChannel setPosition:]` (130) | | |
| `-[OOSoundChannel setGain:]` (135) | | |
| `-[OOSoundChannel playSound:looped:]` (140) | | |
| `-[OOSoundChannel stop]` (159) | | |
| `-[OOSoundChannel hasStopped]` (168) | | |
| `-[OOSoundChannel enqueueBuffer:]` (194) | | |
| `-[OOSoundChannel sound]` (218) | | |

**Differences:**

- *pending*


## OOALSoundDecoder

<a id="ooalsounddecoder"></a>
Upstream `src/Core/OOALSoundDecoder.m` ↔ `Core/OOALSoundDecoder.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOALSoundDecoder initWithPath:]` (60) | | |
| `+[OOALSoundDecoder codecWithPath:]` (72) | | |
| `-[OOALSoundDecoder streamToBuffer:]` (80) | | |
| `-[OOALSoundDecoder readCreatingBuffer:withFrameCount:]` (85) | | |
| `-[OOALSoundDecoder sizeAsBuffer]` (95) | | |
| `-[OOALSoundDecoder isStereo]` (100) | | |
| `-[OOALSoundDecoder sampleRate]` (105) | | |
| `-[OOALSoundDecoder reset]` (110) | | |
| `-[OOALSoundDecoder name]` (115) | | |
| `-[OOALSoundVorbisCodec initWithPath:]` (124) | | |
| `-[OOALSoundVorbisCodec dealloc]` (236) | | |
| `-[OOALSoundVorbisCodec comments]` (251) | | |
| `-[OOALSoundVorbisCodec readCreatingBuffer:withFrameCount:]` (284) | | |
| `-[OOALSoundVorbisCodec streamToBuffer:]` (352) | | |
| `-[OOALSoundVorbisCodec sizeAsBuffer]` (383) | | |
| `-[OOALSoundVorbisCodec isStereo]` (393) | | |
| `-[OOALSoundVorbisCodec description]` (398) | | |
| `-[OOALSoundVorbisCodec sampleRate]` (403) | | |
| `-[OOALSoundVorbisCodec reset]` (408) | | |
| `-[OOALSoundVorbisCodec name]` (431) | | |
| `OOReadOXZVorbis()` (438) | | |
| `OOCloseOXZVorbis()` (455) | | |

**Differences:**

- *pending*


## OOALSoundMixer

<a id="ooalsoundmixer"></a>
Upstream `src/Core/OOALSoundMixer.m` ↔ `Core/OOALSoundMixer.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOSoundMixer sharedMixer]` (39) | | |
| `-[OOSoundMixer init]` (47) | | |
| `-[OOSoundMixer shutdown]` (81) | | |
| `-[OOSoundMixer update]` (89) | | |
| `-[OOSoundMixer popChannel]` (97) | | |
| `-[OOSoundMixer pushChannel:]` (106) | | |
| `+[OOSoundMixer(Singleton) allocWithZone:]` (125) | | |
| `-[OOSoundMixer(Singleton) copyWithZone:]` (134) | | |
| `-[OOSoundMixer(Singleton) retain]` (139) | | |
| `-[OOSoundMixer(Singleton) retainCount]` (144) | | |
| `-[OOSoundMixer(Singleton) release]` (149) | | |
| `-[OOSoundMixer(Singleton) autorelease]` (153) | | |

**Differences:**

- *pending*


## OOALStreamedSound

<a id="ooalstreamedsound"></a>
Upstream `src/Core/OOALStreamedSound.m` ↔ `Core/OOALStreamedSound.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOALStreamedSound dealloc]` (34) | | |
| `-[OOALStreamedSound name]` (43) | | |
| `-[OOALStreamedSound initWithDecoder:]` (48) | | |
| `-[OOALStreamedSound rewind]` (79) | | |
| `-[OOALStreamedSound soundIncomplete]` (85) | | |
| `-[OOALStreamedSound soundBuffer]` (90) | | |

**Differences:**

- *pending*


## OOBasicSoundReferencePoint

<a id="oobasicsoundreferencepoint"></a>
Upstream `src/Core/OOBasicSoundReferencePoint.m` ↔ `Core/OOBasicSoundReferencePoint.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `-[OOSoundReferencePoint setPosition:]` (29) | | |
| `-[OOSoundReferencePoint setVelocity:]` (33) | | |
| `-[OOSoundReferencePoint setOrientation:]` (37) | | |

**Differences:**

- *pending*


## OOMusic

<a id="oomusic"></a>
Upstream `src/Core/OOMusic.m` ↔ *(absorbed — locate in `src/oofnd/` or callers)*

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|

**Differences:**

- *pending*


## OOMusicController

<a id="oomusiccontroller"></a>
Upstream `src/Core/OOMusicController.m` ↔ `Core/OOMusicController.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOMusicController sharedController]` (51) | | |
| `-[OOMusicController init]` (60) | | |
| `-[OOMusicController playMusicNamed:loop:]` (82) | | |
| `-[OOMusicController playMusicNamed:loop:gain:]` (87) | | |
| `-[OOMusicController playThemeMusic]` (106) | | |
| `-[OOMusicController playDockingMusic]` (112) | | |
| `-[OOMusicController playDockedMusic]` (123) | | |
| `-[OOMusicController setMissionMusic:]` (134) | | |
| `-[OOMusicController playMissionMusic]` (140) | | |
| `-[OOMusicController justStop]` (149) | | |
| `-[OOMusicController stop]` (157) | | |
| `-[OOMusicController stopMusicNamed:]` (166) | | |
| `-[OOMusicController stopThemeMusic]` (172) | | |
| `-[OOMusicController stopDockingMusic]` (180) | | |
| `-[OOMusicController stopMissionMusic]` (186) | | |
| `-[OOMusicController toggleDockingMusic]` (192) | | |
| `-[OOMusicController soundSource]` (203) | | |
| `-[OOMusicController playingMusic]` (208) | | |
| `-[OOMusicController isPlaying]` (213) | | |
| `-[OOMusicController mode]` (218) | | |
| `-[OOMusicController setMode:]` (223) | | |
| `+[OOMusicController(Singleton) allocWithZone:]` (282) | | |
| `-[OOMusicController(Singleton) copyWithZone:]` (291) | | |
| `-[OOMusicController(Singleton) retain]` (296) | | |
| `-[OOMusicController(Singleton) retainCount]` (301) | | |
| `-[OOMusicController(Singleton) release]` (306) | | |
| `-[OOMusicController(Singleton) autorelease]` (310) | | |
| `-[OOMusicController(Private) playiTunesPlaylist:]` (320) | | |
| `-[OOMusicController(Private) pauseiTunes]` (342) | | |
| `-[OOMusicController(Private) playiTunesPlaylist:]` (353) | | |
| `-[OOMusicController(Private) pauseiTunes]` (356) | | |

**Differences:**

- *pending*


## OOOpenALController

<a id="ooopenalcontroller"></a>
Upstream `src/Core/OOOpenALController.m` ↔ `Core/OOOpenALController.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOOpenALController sharedController]` (34) | | |
| `-[OOOpenALController init]` (43) | | |
| `-[OOOpenALController setMasterVolume:]` (80) | | |
| `-[OOOpenALController masterVolume]` (85) | | |
| `-[OOOpenALController shutdown]` (94) | | |

**Differences:**

- *pending*


## OOSoundSource

<a id="oosoundsource"></a>
Upstream `src/Core/OOSoundSource.m` ↔ `Core/OOSoundSource.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOSoundSource sourceWithSound:]` (36) | | |
| `-[OOSoundSource init]` (41) | | |
| `-[OOSoundSource initWithSound:]` (53) | | |
| `-[OOSoundSource dealloc]` (64) | | |
| `-[OOSoundSource descriptionComponents]` (72) | | |
| `-[OOSoundSource sound]` (81) | | |
| `-[OOSoundSource setSound:]` (86) | | |
| `-[OOSoundSource loop]` (95) | | |
| `-[OOSoundSource setLoop:]` (100) | | |
| `-[OOSoundSource repeatCount]` (105) | | |
| `-[OOSoundSource setRepeatCount:]` (110) | | |
| `-[OOSoundSource isPlaying]` (115) | | |
| `-[OOSoundSource play]` (120) | | |
| `-[OOSoundSource playOrRepeat]` (148) | | |
| `-[OOSoundSource stop]` (156) | | |
| `+[OOSoundSource stopAll]` (172) | | |
| `-[OOSoundSource playSound:]` (186) | | |
| `-[OOSoundSource playSound:repeatCount:]` (191) | | |
| `-[OOSoundSource playOrRepeatSound:]` (199) | | |
| `-[OOSoundSource setPositional:]` (207) | | |
| `-[OOSoundSource positional]` (220) | | |
| `-[OOSoundSource setPosition:]` (225) | | |
| `-[OOSoundSource position]` (236) | | |
| `-[OOSoundSource setGain:]` (241) | | |
| `-[OOSoundSource gain]` (249) | | |
| `-[OOSoundSource setVelocity:]` (255) | | |
| `-[OOSoundSource setOrientation:]` (259) | | |
| `-[OOSoundSource setConeAngle:]` (263) | | |
| `-[OOSoundSource setGainInsideCone:outsideCone:]` (267) | | |
| `-[OOSoundSource positionRelativeTo:]` (271) | | |
| `-[OOSoundSource channel:didFinishPlayingSound:]` (276) | | |
| `+[OOSoundSource channel:didFinishPlayingSound:]` (293) | | |

**Differences:**

- *pending*


## OOSoundSourcePool

<a id="oosoundsourcepool"></a>
Upstream `src/Core/OOSoundSourcePool.m` ↔ `Core/OOSoundSourcePool.cpp`

**Status:** pending

| Upstream (line) | C++ counterpart | Verdict |
|---|---|---|
| `+[OOSoundSourcePool poolWithCount:minRepeatTime:]` (50) | | |
| `-[OOSoundSourcePool initWithCount:minRepeatTime:]` (55) | | |
| `-[OOSoundSourcePool dealloc]` (80) | | |
| `-[OOSoundSourcePool playSoundWithKey:]` (94) | | |
| `-[OOSoundSourcePool playSoundWithKey:]` (154) | | |
| `-[OOSoundSourcePool playSoundWithKey:]` (165) | | |
| `-[OOSoundSourcePool playSoundWithKey:]` (176) | | |
| `-[OOSoundSourcePool playSoundWithKey:]` (184) | | |
| `-[OOSoundSourcePool playSoundWithKey:position:]` (189) | | |
| `-[OOSoundSourcePool playSoundWithKey:overlap:]` (194) | | |
| `-[OOSoundSourcePool playSoundWithKey:overlap:position:]` (203) | | |
| `-[OOSoundSourcePool(Private) selectSlotForPriority:]` (216) | | |

**Differences:**

- *pending*

