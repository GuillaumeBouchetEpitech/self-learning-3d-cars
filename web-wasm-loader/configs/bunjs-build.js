
import * as fs from 'fs';
import * as path from 'path';

// console.log('process.argv', process.argv);

const _getBuildOptions = () => {
  const buildOptionsRegex = /build-options-(debug|release)/;

  // start at 2 (since 0 is "bun", and 1 is "bun-build.js")
  for (let ii = 2; ii < process.argv.length; ++ii) {
    const capture = buildOptionsRegex.exec(process.argv[ii]);
    if (!capture) {
      continue;
    }

    return { isRelease: capture[1] === 'release' };
  }

  throw new Error('missing build options argument, stopping now');
};

const buildOptions = _getBuildOptions();

//
//
//

const asyncBuild = async ({
  name,
  // tsConfigFilePath,
  inputFilePath,
  outputFilePath,
}) => {

  console.log(` -> BUILDING ${name}`);
  const startTime = Date.now();

  const config = {
    entrypoints: [inputFilePath],
    outdir: '.',
    target: 'browser',
    format: "esm",
    root: path.dirname(inputFilePath),
    naming: outputFilePath,
  };

  if (buildOptions.isRelease) {
    config.minify = {
      whitespace: true,
      identifiers: true,
      syntax: true,
    }
  } else {
    config.sourcemap = "inline";
  }

  const result = await Bun.build(config);

  if (!result || result.success === false) {
    console.log('ERROR.result', result);
    return;
  }

  const endTime = Date.now();
  const elapsedTime = ((endTime - startTime) / 1000).toFixed(3);

  console.log(`    -> BUILT ${name} (${elapsedTime}sec)`);
  const statData = fs.statSync(outputFilePath);
  console.log(`      -> SIZE ${Math.ceil(statData.size / 1024)}ko`);
};

const asyncRun = async () => {
  await Promise.all([
    asyncBuild({
      name: 'main',
      // tsConfigFilePath: `./src/main/tsconfig.json`,
      inputFilePath: `./src/main.ts`,
      outputFilePath: `./js/bundle.js`,
    }),
  ]);
};
asyncRun();
