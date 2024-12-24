def builds = [
              x86_64_us: ['device': 'x86_64', 'libc': 'musl', 'region': 'us', 'dpdk': 'false'],
              x86_64_eu: ['device': 'x86_64', 'libc': 'musl', 'region': 'eu', 'dpdk': 'false'],
] 

def jobs = [:] // dynamically populated later on

def credentialsId = 'buildbot'

void buildMFW(String device, String libc, String region, String startClean, String makeOptions, String dpdkFlag, String buildBranch, String toolsDir, String credentialsId) {
  sshagent (credentials:[credentialsId]) {
    sh "docker-compose -f ${toolsDir}/docker-compose.build.yml -p mfw_${device}_${region} build build-local-container"
    sh "docker-compose -f ${toolsDir}/docker-compose.build.yml -p mfw_${device}_${region} run build-local-container ${dpdkFlag} -d ${device} -l ${libc} -r ${region} -c ${startClean} -m '${makeOptions}' -v ${buildBranch}"
  }
}

void archiveMFW(String device, String region, String toolsDir, String artifactsDir) {
  sh "rm -fr ${artifactsDir}"
  sh "${toolsDir}/version-images.sh -d ${device} -r ${region} -o ${artifactsDir} -c -t \$(cat tmp/version.date)"
}

pipeline {
  agent none

  options {
    disableConcurrentBuilds()
    quietPeriod(60)
  }

  triggers {
    upstream(upstreamProjects:"packetd/${env.BRANCH_NAME}, reportd/${env.BRANCH_NAME}, restd/${env.BRANCH_NAME}, sync-settings/${env.BRANCH_NAME}, classd/${env.BRANCH_NAME}, bctid/${env.BRANCH_NAME}, feeds/${env.BRANCH_NAME}, admin/${env.BRANCH_NAME}, mfw_ui/${env.BRANCH_NAME}, mfw_build/${env.BRANCH_NAME}, bpfgen/${env.BRANCH_NAME}, client-license-service/${env.BRANCH_NAME}, support-diagnostics/${env.BRANCH_NAME}, discoverd/${env.BRANCH_NAME}, secret-manager/${env.BRANCH_NAME}, wan-utils/${env.BRANCH_NAME}",
             threshold: hudson.model.Result.SUCCESS)
  }

  parameters {
    string(name:'buildBranch', defaultValue:env.BRANCH_NAME, description:'branch to use for feeds.git and mfw_build.git')
    choice(name:'startClean', choices:['false', 'true'], description:'start clean')
    choice(name:'onlyBuildUS', choices:['true', 'false'], description:'build only US images (no EU or other regions)')
    string(name:'makeOptions', defaultValue:'-j32', description:'options passed directly to make')
  }

  stages {
    stage('Build') {
      steps {
        script {
          builds.each { build ->
            def myDevice = build.value.device
            def myRegion = build.value.region
            def libc = build.value.libc
            def jobName = build.key
            def option = ""
            def dpdkFlag = ""

            if (myRegion != 'us' && onlyBuildUS == 'true') {
              return
            }

            echo "Adding job ${build.key}"
            jobs[build.key] = {
              node('mfw') {
                stage(jobName) {
                  def artifactsDir = "tmp/artifacts"

                  // default values for US build
                  def buildDir = "${env.HOME}/build-mfw-${buildBranch}-${myDevice}"
                  def toolsDir = "${env.HOME}/tools-mfw-${buildBranch}-${myDevice}"

                  if (myRegion != 'us') {
                    buildDir = buildDir + "-" + myRegion
                    toolsDir = toolsDir + "-" + myRegion
                  }
                  
                  if (build.value.dpdk == 'true') {
                    dpdkFlag = "--with-dpdk"
                    option = "dpdk"
                    buildDir = buildDir + "-dpdk"
                    toolsDir = toolsDir + "-dpdk"
                    artifactsDir = artifactsDir + "/dpdk"
                  }

                  if (buildBranch =~ /^mfw\+owrt/) {
                    // force master
                    branch = 'master'
                  } else {
                    branch = buildBranch
                  }
                  echo "Building ${build.key} with branch ${branch}"
                  dir(toolsDir) {
                    git url:"git@github.com:untangle/mfw_build", branch:branch, credentialsId:credentialsId
                  }
                  dir(buildDir) {
                    checkout scm

                    buildMFW(myDevice, libc, myRegion, startClean, makeOptions, dpdkFlag, branch, toolsDir, credentialsId)

                    if (myDevice == 'x86_64' && myRegion == 'us' && build.value.dpdk == 'false') {
                      stash(name:"rootfs-${myDevice}", includes:"bin/targets/**/*generic-rootfs.tar.gz")
                    }

                    archiveMFW(myDevice, myRegion, toolsDir, "${env.WORKSPACE}/${artifactsDir}")
                  }
                  archiveArtifacts artifacts:"${artifactsDir}/**/*", fingerprint:true
                }
              }
            } // jobs
          } // for loop
        parallel jobs
        } // script
      } // steps
      post {
        changed {
          script {
            // set result before pipeline ends, so emailer sees it
            currentBuild.result = currentBuild.currentResult
          }
          emailext(to:'nfgw-engineering@untangle.com', subject:"${env.JOB_NAME} #${env.BUILD_NUMBER}: ${currentBuild.result}", body:"${env.BUILD_URL}")
          slackSend(channel:"#team_engineering", message:"${env.JOB_NAME} #${env.BUILD_NUMBER}: ${currentBuild.result} at ${env.BUILD_URL}")
        }
      }
  	} // stage

    stage('Test') {
      parallel {
        stage('Test x86_64') {
          agent { label 'mfw' }

          environment {
            device = 'x86_64'
            toolsDir = "${env.HOME}/tools-mfw-${buildBranch}-${device}"
            rootfsTarballName = 'mfw-x86-64-generic-rootfs.tar.gz'
            rootfsTarballPath = "bin/targets/x86/64/${rootfsTarballName}"
            dockerfile = "${toolsDir}/docker-compose.test.yml"
          }

          stages {
            stage('Prep x86_64') {
              steps {
                script {
                  if (buildBranch =~ /^mfw\+owrt/) {
                    // force master
                    branch = 'master'
                  } else {
                    branch = buildBranch
                  }

                  dir(toolsDir) {
                    git url:"git@github.com:untangle/mfw_build", branch:branch, credentialsId:credentialsId
                  }

                  unstash(name:"rootfs-${device}")
                  sh("test -f ${rootfsTarballPath}")
                  sh("mv -f ${rootfsTarballPath} ${toolsDir}")
                }
              }
            }

            stage('TCP services') {
              steps {
                dir('mfw') {
                  script {
                    try {
                      sh("docker-compose -f ${dockerfile} build --build-arg ROOTFS_TARBALL=${rootfsTarballName} mfw")
                      sh("docker-compose -f ${dockerfile} up --abort-on-container-exit --exit-code-from test")
                    } catch (exc) {
                      currentBuild.result = 'UNSTABLE'
                      unstable('TCP services test failed')
                    }
                  }
                }
              }
            }
          } // stages
        } // stage
      } // parallel

      post {
        changed {
          script {
            // set result before pipeline ends, so emailer sees it
            currentBuild.result = currentBuild.currentResult
          }
          emailext(to:'nfgw-engineering@untangle.com', subject:"${env.JOB_NAME} #${env.BUILD_NUMBER}: ${currentBuild.result}", body:"${env.BUILD_URL}")
          slackSend(channel:"#team_engineering", message:"${env.JOB_NAME} #${env.BUILD_NUMBER}: ${currentBuild.result} at ${env.BUILD_URL}")
        }
      }
    } // stage Test
  } // stages
} // pipeline

