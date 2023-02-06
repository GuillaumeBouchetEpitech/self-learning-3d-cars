
import typescript from '@rollup/plugin-typescript';
import commonjs from '@rollup/plugin-commonjs';
import terser from '@rollup/plugin-terser';

const plugins = [
  typescript(),
  commonjs(),
];

if (process.env["BUILD_MODE"] !== 'DEBUG') {
  plugins.push(terser());
}

export default {
  input: 'src/main.ts',
  output: {
    file: './js/bundle.js',
    format: 'cjs',
  },
  plugins,
};
