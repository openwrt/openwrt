#!/usr/bin/env groovy

/**
 * Build openwrt project
 *
 * Configure as needed, pull in the feeds and run the build. Allows
 * overridding feeds listed using build parameters.
 * 
 * Note this script needs some extra Jenkins permissions to be
 * approved.
 */
// TODO be careful with using dir() until JENKINS-33510 is fixed

def customFeeds = [
    ['packages', 'packages', 'https://github.com/CreatorDev'],
    ['ci40', 'ci40-platform-feed', 'https://github.com/CreatorDev'],
    ['creator', 'creator-feed', 'https://github.com/CreatorDev'],
]
def feedParams = []
for (feed in customFeeds) {
    feedParams.add(string(defaultValue: '', description: 'Branch/commmit/PR to override feed. \
        (Can be a PR using PR-< id >)', name: "OVERRIDE_${feed[1].toUpperCase()}"))
}

properties([
    buildDiscarder(logRotator(numToKeepStr: '5')),
    parameters([
        booleanParam(defaultValue: false,
            description: 'Build extra tools such as toolchain, SDK and Image builder',
            name: 'BUILD_TOOLS'),
        booleanParam(defaultValue: false, description: 'Build *all* packages for opkg',
            name: 'ALL_PACKAGES'),
        stringParam(defaultValue: 'target/linux/pistachio/creator-platform-default-cascoda.config',
            description: 'Config file to use', name: "CONFIG_FILE"),
        stringParam(defaultValue: '', description: 'Set version, if blank job number will be used.',
            name: 'VERSION'),
    ] + feedParams)
])

node('docker && imgtec') {  // Only run on internal slaves as build takes a lot of resources
    def docker_image

    stage('Prepare Docker container') {
        // Setup a local docker container to run build on this slave
        docker_image = docker.image "imgtec/creator-builder:latest" // TODO for now have manually setup on slave
    }

    docker_image.inside("-v ${WORKSPACE}/../../reference-repos:${WORKSPACE}/../../reference-repos:ro") {
        stage('Configure') {
            // Checkout a clean version of the repo using reference repo to save bandwidth/time
            checkout([$class: 'GitSCM',
                userRemoteConfigs: scm.userRemoteConfigs,
                branches: scm.branches,
                doGenerateSubmoduleConfigurations: scm.doGenerateSubmoduleConfigurations,
                submoduleCfg: scm.submoduleCfg,
                browser: scm.browser,
                gitTool: scm.gitTool,
                extensions: scm.extensions + [
                    [$class: 'CleanCheckout'],
                    [$class: 'PruneStaleBranch'],
                    [$class: 'CloneOption', honorRefspec: true, reference: "${WORKSPACE}/../../reference-repos/openwrt.git"],
                ],
            ])

            // Default config
            sh "cp ${params.CONFIG_FILE?.trim()} .config"

            // Versioning
            sh "sed -i 's/.*CONFIG_VERSION_NUMBER.*/CONFIG_VERSION_NUMBER=\"${params.VERSION?.trim() ?: 'j' + env.BUILD_NUMBER}\"/g' .config"
            sh 'scripts/getver.sh > version'

            // Build tools/sdks
            if (params.BUILD_TOOLS) {
                echo 'Enabling toolchain, image builder and sdk creation'
                sh 'echo \'' \
                 + 'CONFIG_MAKE_TOOLCHAIN=y\n' \
                 + 'CONFIG_IB=y\n' \
                 + 'CONFIG_SDK=y\n' \
                 + '\' >> .config'
            }

            // Build all (for opkg)
            // TODO grab vault creds and mod config to use OPKGSMIME
            if (params.ALL_PACKAGES){
                echo 'Enabling all user and kernel packages'
                sh 'echo \'' \
                 + 'CONFIG_ALL=y\n' \
                 + '\' >> .config'
            }

            // Boardfarm-able
            // TODO work out which ones we actually have
            echo 'Enabling usb ethernet adapters modules (for boardfarm testing)'
            sh 'echo \'' \
             + 'CONFIG_PACKAGE_kmod-usb-net=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-asix=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-cdc-eem=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-cdc-ether=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-cdc-mbim=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-cdc-ncm=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-cdc-subset=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-dm9601-ether=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-hso=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-huawei-cdc-ncm=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-ipheth=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-kalmia=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-kaweth=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-mcs7830=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-pegasus=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-qmi-wwa=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-rndis=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-rtl8150=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-rtl8152=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-sierrawireless=y\n' \
             + 'CONFIG_PACKAGE_kmod-usb-net-smsc95xx=y\n' \
             + '\' >> .config'

            // If specified override each feed with local clone
            for (feed in customFeeds) {
                if (params."OVERRIDE_${feed[1].toUpperCase()}"?.trim()){
                    dir("feed-${feed[1]}") {
                        checkout([
                            $class: 'GitSCM',
                            branches: [[name: env."OVERRIDE_${feed[1].toUpperCase()}"]],
                            userRemoteConfigs: [[
                                refspec: '+refs/pull/*/head:refs/remotes/origin/PR-* \
                                    +refs/heads/*:refs/remotes/origin/*',
                                url: "${feed[2]}/${feed[1]}.git"
                            ]]
                        ])
                    }
                    // Replace (or add if not exists) feed with local clone
                    sh "grep -q 'src-.* ${feed[0]} .*' feeds.conf.default && \
                        sed -i 's|^.*\\s\\(${feed[0]}\\)\\s.*\$|src-link \\1 ../feed-${feed[1]}|g' feeds.conf.default || \
                        echo 'src-link ${feed[0]} ../feed-${feed[1]}' >> feeds.conf.default"
                }
            }
            sh 'cat .config'
            sh 'cat feeds.conf.default'
            sh 'scripts/feeds update -a && scripts/feeds install -a'
            sh 'make defconfig'
        }
        stage('Build') {
            // Attempt to build quickly and reliably
            try {
                sh "make -j4 V=s ${params.ALL_PACKAGES ? 'IGNORE_ERRORS=m' : ''}"
            } catch (hudson.AbortException err) {
                // TODO BUG JENKINS-28822
                if(err.getMessage().contains('script returned exit code 143')) {
                    throw err
                }
                echo 'Parallel build failed, attempting to continue in  single threaded mode'
            }
            sh "make -j1 V=s ${params.ALL_PACKAGES ? 'IGNORE_ERRORS=m' : ''}"
        }

        stage('Upload') {
            archiveArtifacts 'bin/*/*'
            if (params.ALL_PACKAGES) {
                archiveArtifacts 'bin/*/packages/**'
            }
            deleteDir()  // clean up the workspace to save space
        }
    }
}
//node('boardfarm') {
    stage('Intergration test') {
        //TODO run the boardfarm test
    }
//}
