import QtQuick 2.15
import QtQuick.Particles
import Qt5Compat.GraphicalEffects

Item  {
   id: particlesContainer

   required property var effectRGBList
   required property var windowObject
   required property string core
   required property real colorVariationValue
   required property real containterSize
   required property real bottomMargin
   required property string type
   required property string judgementVal
   property real lifetime: 500

   width: containterSize
   height: containterSize
   anchors.bottom: parent.bottom
   anchors.bottomMargin: bottomMargin - height/2

   z: 2

   Timer{
      interval: lifetime
      onTriggered: kill()
      running: true
   }

   function kill(){
      destroy()
   }

   ParticleSystem{
      id: explosionSystem
      anchors.fill: parent
   }
   ParticleSystem{
      id: shineSystem
      anchors.fill: parent
   }

   ParticleSystem{
      id: waveSystem
      anchors.fill: parent
   }

   Emitter{
      id: explosionEmitter
      system: explosionSystem
      anchors.centerIn: parent
      lifeSpan: 200
      emitRate: 2
      size: parent.height * .1

      velocity: CumulativeDirection{
         AngleDirection{
            angleVariation:  360
            magnitude: windowObject.height * 0.9
            magnitudeVariation: 50
         }
      }

      onEmitParticles: (particles) => {
          for (var i=0; i<particles.length; i++) {
                             let particle = particles[i];

                             if (judgementVal != "Perfect" && type !== "hold"  && type !== "slide"){
                                particle.red = effectRGBList[0]
                                particle.green = effectRGBList[1]
                                particle.blue = effectRGBList[2]
                             }
                             else{
                                particle.red = Math.random()
                                particle.green = Math.random()
                                particle.blue = Math.random()
                             }



                          }

                          }

      ImageParticle {
         id: explosionImg
         system: explosionSystem
         source: "Particles/ParticleDiamond.png"   // use a glowing dot or spark

         entryEffect: ImageParticle.Fade
         Component.onCompleted: {
            explosionEmitter.burst(12)

         }

      }






   }

   Emitter{
      id: waveEmitter
      system: waveSystem
      anchors.centerIn: parent

      lifeSpan: 120
      emitRate: 0
      size: parent.width
      endSize: parent.width * 2

      onEmitParticles: (particles) => {
          for (var i=0; i<particles.length; i++) {
                             let particle = particles[i];

                             if (judgementVal != "Perfect" && type !== "hold"  && type !== "slide"){
                                particle.red = effectRGBList[0]
                                particle.green = effectRGBList[1]
                                particle.blue = effectRGBList[2]
                             }




                          }

                          }

      velocity: PointDirection {
         x: 0
         y: 0
      }

      ImageParticle {
         id: waveImg
         system: waveSystem
         source: core
         opacity: 0.2
         entryEffect: ImageParticle.Fade


         Component.onCompleted: {
            waveEmitter.burst(12)


         }
      }

   }

}
